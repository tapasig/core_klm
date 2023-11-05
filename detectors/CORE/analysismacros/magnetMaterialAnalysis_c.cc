#include "magnetMaterialAnalysis.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <stdio.h>

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

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <TFile.h>
#include <TNtuple.h>

#include <cassert>
#include <sstream>
#include <string>
#include <iostream>

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

    g4hitntuple_inner = new TNtuple("hitntup_inner", "G4Hits", "x0:y0:z0:x1:y1:z1:edep:px:py:t:trkid:hitid");
    g4hitntuple_outer = new TNtuple("hitntup_outer", "G4Hits", "x0:y0:z0:x1:y1:z1:edep:px:py:t:trkid:hitid");

    std::cout << "magnetMaterialAnalysis::Init(PHCompositeNode *topNode) Initializing" << std::endl;

        
    gDirectory->mkdir("VirtualDetector");
    gDirectory->cd("VirtualDetector");

    x_inner = new TH1F("x_inner", "x_inner", 250, -20, 30);
    x_outer = new TH1F("x_outer", "x_outer", 250, -20, 30);
    x_diff = new TH1F("x_diff", "x_diff", 250, -20, 30);

    // h2_VD_XY  = new TH2F("VD_XY", "VD_XY", 50, 0, 50, 300, 0, 300);
    // h2_VD_pXpY = new TH2F("VD_pxpy", "px vs. py", 50, 0, 50, 300, 0, 300);

    gDirectory->cd("/");

    return Fun4AllReturnCodes::EVENT_OK;
}


int magnetMaterialAnalysis::process_event(PHCompositeNode *topNode)
{
    std::cout << "magnetMaterialAnalysis::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  process_g4hits_inner(topNode, "MAGNET_DET_INNER");
  process_g4hits_outer(topNode, "MAGNET_DET_OUTER");

  analysis(topNode);

    return Fun4AllReturnCodes::EVENT_OK;

}

int magnetMaterialAnalysis::End(PHCompositeNode *topNode)
{
    
    std::cout << "magnetMaterialAnalysis::End(PHCompositeNode *topNode) This is the End..." << std::endl;

    outfile->cd();
    // g4hitntuple->Write();
    outfile->Write();
    outfile->Close();
    delete outfile;
    hm->dumpHistos(outfilename, "UPDATE");

    return Fun4AllReturnCodes::EVENT_OK;
}


int magnetMaterialAnalysis::process_g4hits_inner(PHCompositeNode* topNode, const std::string& detector)
{

   // loop over the G4Hits

    ostringstream nodename;
    nodename.str("");
    nodename << "G4HIT_" << detector;
    cout << nodename.str().c_str() << endl;
    PHG4HitContainer* hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());
    

    //PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, nodename.str().c_str());
    //PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
    //cout << truthinfo << endl;

    // PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
    /*
    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!= range.second; ++iter)
    {
      PHG4Particle* particle = iter->second;
      int particleID = particle->get_pid();
      cout << particleID << endl;
    }*/

    if (hits)
    {

        // this returns an iterator to the beginning and the end of our G4Hits
        PHG4HitContainer::ConstRange hit_range = hits->getHits();

       
        for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
        {
            
            // Fill the x inner array
            x_inner->Fill(hit_iter->second->get_x(0));

            // the pointer to the G4Hit is hit_iter->second
            g4hitntuple_inner->Fill(hit_iter->second->get_x(0),
                              hit_iter->second->get_y(0),
                              hit_iter->second->get_z(0),
                              hit_iter->second->get_x(1),
                              hit_iter->second->get_y(1),
                              hit_iter->second->get_z(1),
                              hit_iter->second->get_edep(),
			      hit_iter->second->get_px(0),
			      hit_iter->second->get_py(0),
			      hit_iter->second->get_t(0),
                              hit_iter->second->get_trkid(),
                              hit_iter->second->get_hit_id());
           

	    // h2_VD_XY->Fill(hit_iter->second->get_x(0), hit_iter->second->get_y(0));
	    // h2_VD_pXpY->Fill(hit_iter->second->get_px(0), hit_iter->second->get_py(0));

//            cout << "pid: " << particle->get_pid() << endl;
/*
            // cout << "TYPE: " << hit_iter->second->get_hit_type() << endl;
            cout << "\n" << endl;
            hit_iter->second->identify();
            //hit_iter->second->print();
            // cout << hit_iter->second->get_shower_id() << endl;
            cout << "trkid:" << hit_iter->second->get_trkid() << endl;
            cout << "hitid:" << hit_iter->second->get_hit_id() << endl;
            //// cout << is_primary(hit_iter->second) << endl;
            cout << "\n" << endl;
*/
        }

    }
    // Get the entries for x0
    // xa_inner = g4hitntuple_inner->GetEntries("x0");
    // x_inner->Fit("gaus");

    return Fun4AllReturnCodes::EVENT_OK;
}


int magnetMaterialAnalysis::process_g4hits_outer(PHCompositeNode* topNode, const std::string& detector)
{

   // loop over the G4Hits
    ostringstream nodename;
    nodename.str("");
    nodename << "G4HIT_" << detector;
    cout << nodename.str().c_str() << endl;
    PHG4HitContainer* hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());
    

    //PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, nodename.str().c_str());
    //PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
    //cout << truthinfo << endl;

    // PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
    /*
    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!= range.second; ++iter)
    {
      PHG4Particle* particle = iter->second;
      int particleID = particle->get_pid();
      cout << particleID << endl;
    }*/

    if (hits)
    {

        // this returns an iterator to the beginning and the end of our G4Hits
        PHG4HitContainer::ConstRange hit_range = hits->getHits();

       
        for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
        {

            // Fill the x outer array
            x_outer->Fill(hit_iter->second->get_x(0));

            // the pointer to the G4Hit is hit_iter->second
            g4hitntuple_outer->Fill(hit_iter->second->get_x(0),
                              hit_iter->second->get_y(0),
                              hit_iter->second->get_z(0),
                              hit_iter->second->get_x(1),
                              hit_iter->second->get_y(1),
                              hit_iter->second->get_z(1),
                              hit_iter->second->get_edep(),
			      hit_iter->second->get_px(0),
			      hit_iter->second->get_py(0),
			      hit_iter->second->get_t(0),
                              hit_iter->second->get_trkid(),
                              hit_iter->second->get_hit_id());
            

	    // h2_VD_XY->Fill(hit_iter->second->get_x(0), hit_iter->second->get_y(0));
	    // h2_VD_pXpY->Fill(hit_iter->second->get_px(0), hit_iter->second->get_py(0));

//            cout << "pid: " << particle->get_pid() << endl;
/*
            // cout << "TYPE: " << hit_iter->second->get_hit_type() << endl;
            cout << "\n" << endl;
            hit_iter->second->identify();
            //hit_iter->second->print();
            // cout << hit_iter->second->get_shower_id() << endl;
            cout << "trkid:" << hit_iter->second->get_trkid() << endl;
            cout << "hitid:" << hit_iter->second->get_hit_id() << endl;
            //// cout << is_primary(hit_iter->second) << endl;
            cout << "\n" << endl;
*/
        }

    }
    
    // Get the entries for x0
    // xa_outer = g4hitntuple_outer->GetEntries("x0");
    // x_outer->Fit("gaus");

    return Fun4AllReturnCodes::EVENT_OK;
}


int magnetMaterialAnalysis::analysis(PHCompositeNode* topNode)
{
    // x_diff->SetNameTitle("x_difference", "x_difference");
    // x_diff = (TH1F*) x_inner->Clone("x_diff");
    
    x_diff->Add(x_outer, x_inner, -1.0, 1.0);
    // x_diff->SetMinimum(0.0);
    // x_diff->Fit("gaus");
    
    // x_diff = x_inner->Add(x_outer, -1);
    
    // xa_diff = xa_outer - xa_inner;
    // int xa_diff_size = sizeof(xa_diff)/sizeof(double);

    // x_diff->Fill(xa_diff);
    
    TList *list = new TList;
    list->Add(x_inner);

    TH1F *x_inner_c = (TH1F*)x_inner->Clone("x_innerClone");
    x_inner_c->Reset();
    x_inner_c->Merge(list);
    x_inner_c->Draw();
    

    return Fun4AllReturnCodes::EVENT_OK;

}
