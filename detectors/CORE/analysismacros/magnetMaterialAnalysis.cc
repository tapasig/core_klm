#include "magnetMaterialAnalysis.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <stdio.h>
#include <TCanvas.h>
#include <TROOT.h>

#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>  // for PHNode
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>  // for PHObject
#include <phool/PHRandomSeed.h>
#include <phool/getClass.h>
#include <phool/phool.h>

// G4Hits includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

// Histogram include
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH1F.h"
#include "TList.h"

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <TFile.h>
#include <TNtuple.h>

#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

#include <gsl/gsl_randist.h>

#include <gsl/gsl_rng.h>

// /// HEPMC truth includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

/// Fun4All includes
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4main/PHG4Reco.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

#include <pdbcalbase/PdbParameterMapContainer.h>

#include <g4eval/SvtxEvalStack.h>

using namespace std;

//ofstream myfile;


magnetMaterialAnalysis::magnetMaterialAnalysis(const std::string &name, const std::string& filename):
 SubsysReco(name)
 , outfilename(filename)
{
    std::cout << "magnetMaterialAnalysis" << std::endl;
}


magnetMaterialAnalysis::~magnetMaterialAnalysis()
{
    std::cout << "magnetMaterialAnalysis::~magnetMaterialAnalysis Calling dtor" << std::endl;
}


int magnetMaterialAnalysis::Init(PHCompositeNode *topNode)
{
    hm = new Fun4AllHistoManager(Name());

    outfile = new TFile(outfilename.c_str(), "RECREATE");

    g4hitntuple_inner = new TNtuple("hitntup_inner", "G4Hits", "x0:y0:z0:x1:y1:z1:edep:px:py:pz");
    g4hitntuple_outer = new TNtuple("hitntup_outer", "G4Hits", "x0:y0:z0:x1:y1:z1:edep:px:py:pz");
    g4hitntuple_diff = new TNtuple("hitntup_diff", "G4Hits", "x_diff:phi_delta:theta_delta:p_delta");

    std::cout << "magnetMaterialAnalysis::Init(PHCompositeNode *topNode) Initializing" << std::endl;
/*
    // Open the data write text file...
    //myfile.open("output.txt");
//    myfile << "x_inner" << "\t" << "x_outer" << "\t" << "x_diff" << "\t" 
//           << "phi_inner" << "\t" << "phi_outer" << "\t" << "phi_diff" << "\t"
//           << "theta_inner" << "\t" << "theta_outer" << "\t" << "theta_diff" << "\n"; 
*/
        
    gDirectory->mkdir("VirtualDetector");
    gDirectory->cd("VirtualDetector");

   // Histograms with auotomatic binning
    x_diff = new TH1F("x_diff", "x_diff", 100, 10, 10);
    xin1_xout0 = new TH2F("xin1_xout0", "x_in_1 vs x_out_0", 150, 10, 10, 250, 10, 10); 
    phi_delta = new TH1F("phi_delta", "phi_delta", 100, 10, 10);
    theta_delta = new TH1F("theta_delta", "theta_delta", 100, 10, 10);

    gDirectory->cd("/");

    return Fun4AllReturnCodes::EVENT_OK;
}


int magnetMaterialAnalysis::process_event(PHCompositeNode *topNode)
{
    std::cout << "magnetMaterialAnalysis::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

    process_g4hits(topNode, "MAGNET_DET_INNER", "MAGNET_DET_OUTER");


    return Fun4AllReturnCodes::EVENT_OK;
}

int magnetMaterialAnalysis::End(PHCompositeNode *topNode)
{

    x_diff->Fit("gaus", "", "", -100, 220);
    phi_delta->Fit("gaus", "", "", -10.0, 5.0);
    theta_delta->Fit("gaus", "", "", -2.3, -0.5);

     
    std::cout << "magnetMaterialAnalysis::End(PHCompositeNode *topNode) This is the End..." << std::endl;

//    myfile.close();

    outfile->cd();
    outfile->Write();
    outfile->Close();
    delete outfile;
    hm->dumpHistos(outfilename, "UPDATE");

    return Fun4AllReturnCodes::EVENT_OK;
}


int magnetMaterialAnalysis::process_g4hits(PHCompositeNode* topNode, const std::string& detector_inner, const std::string& detector_outer)
{

   // loop over the G4Hits

    ostringstream nodename;
    nodename.str("");
    nodename << "G4HIT_" << detector_inner;
    cout << "Inner detector: " << nodename.str().c_str() << endl;
    PHG4HitContainer* hits_inner = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());

    // ostringstream nodename;
    nodename.str("");
    nodename << "G4HIT_" << detector_outer;
    cout << "Outer detector: " << nodename.str().c_str() << endl;
    PHG4HitContainer* hits_outer = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());

    if (hits_outer && hits_inner)
    {

        // this returns an iterator to the beginning and the end of our G4Hits
        PHG4HitContainer::ConstRange hit_range_inner = hits_inner->getHits();
        PHG4HitContainer::ConstRange hit_range_outer = hits_outer->getHits();

        PHG4HitContainer::ConstIterator hit_iter_inner; 
        PHG4HitContainer::ConstIterator hit_iter_outer; 

        // for-loop tracking two variables
        for (hit_iter_inner = hit_range_inner.first, hit_iter_outer = hit_range_outer.first; 
            (hit_iter_inner != hit_range_inner.second && hit_iter_outer != hit_range_outer.second); 
            hit_iter_inner++, hit_iter_outer++)
        {
            
            // phi analysis-----------------
            // Get phi from px_inner_1, py_inner_1
            phi_inner = atan(hit_iter_inner->second->get_py(1)/        hit_iter_inner->second->get_px(1));
        
            // Get phi from px_outer_0, py_outer_0
            phi_outer = atan(hit_iter_outer->second->get_py(0)/        hit_iter_outer->second->get_px(0));

            // Calculate their difference and
            // store this delta phi (for this specific event) in a custom histogram
            phi_delta->Fill(phi_outer-phi_inner);
            // -------------------


            // theta analysis-----------------
            // Get theta from px_inner_1, py_inner_1, pz_inner_1
            pt_inner = sqrt(
                (hit_iter_inner->second->get_px(1)*hit_iter_inner->second->get_px(1)) + (hit_iter_inner->second->get_py(1)*hit_iter_inner->second->get_py(1))
                );
            theta_inner = acos(hit_iter_inner->second->get_pz(1)/pt_inner);
        
            // Get theta from px_outer_0, py_outer_0, pz_inner_0
            pt_outer = sqrt(
                (hit_iter_outer->second->get_px(0)*hit_iter_outer->second->get_px(0)) + (hit_iter_outer->second->get_py(0)*hit_iter_outer->second->get_py(0))
                );
            theta_outer = acos(hit_iter_outer->second->get_pz(0)/pt_outer);

            // Calculate their difference and
            // store this delta theta (for this specific event) in a custom histogram
            theta_delta->Fill(theta_outer-theta_inner);
            // -------------------
            
            // position analysis-----------------
            // Compute the delta x and store that into 1D histogram...
            x_diff_val = hit_iter_inner->second->get_x(1) - hit_iter_outer->second->get_x(0);
            x_diff->Fill(x_diff_val);
            // -------------------


//
//            // Write the all values to file...
//            myfile << hit_iter_inner->second->get_x(1) << "\t" << hit_iter_outer->second->get_x(0) << "\t " << x_diff_val << "\t"
//                   << phi_inner << "\t" << phi_outer << "\t" << phi_outer-phi_inner << "\t"
//                   << theta_inner << "\t" << theta_outer << "\t" << theta_outer-theta_inner << "\n";
//

            // Drawing 2D histograms...
            xin1_xout0->Fill(hit_iter_inner->second->get_x(1), hit_iter_outer->second->get_x(0));


            // the pointer to the G4Hit is hit_iter->second
            g4hitntuple_inner->Fill(hit_iter_inner->second->get_x(0),
                              hit_iter_inner->second->get_y(0),
                              hit_iter_inner->second->get_z(0),
                              hit_iter_inner->second->get_x(1),
                              hit_iter_inner->second->get_y(1),
                              hit_iter_inner->second->get_z(1),
                              hit_iter_inner->second->get_edep(),
                              hit_iter_inner->second->get_px(0),
			      hit_iter_inner->second->get_py(0),
			      hit_iter_inner->second->get_pz(0));

            g4hitntuple_outer->Fill(hit_iter_outer->second->get_x(0),
                              hit_iter_outer->second->get_y(0),
                              hit_iter_outer->second->get_z(0),
                              hit_iter_outer->second->get_x(1),
                              hit_iter_outer->second->get_y(1),
                              hit_iter_outer->second->get_z(1),
                              hit_iter_outer->second->get_edep(),
                              hit_iter_outer->second->get_px(0),
			      hit_iter_outer->second->get_py(0),
                              hit_iter_outer->second->get_pz(0));

            g4hitntuple_diff->Fill(x_diff_val,
				   phi_outer-phi_inner,
  				   theta_outer-theta_inner,pt_outer - pt_inner);
           
        }
    }

    return Fun4AllReturnCodes::EVENT_OK;
}
