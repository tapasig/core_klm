#ifndef MAGNETMATERIALANALYSIS_H
#define MAGNETMATERIALANALYSIS_H

#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Reco.h>

#include <string>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"

#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TNtuple;


class magnetMaterialAnalysis : public SubsysReco
{
    public:

    magnetMaterialAnalysis(const std::string &name = "Analysis_Macro", const std::string &fname = "MyNtuple.root");

    virtual ~magnetMaterialAnalysis();

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
    */
    int Init(PHCompositeNode *topNode) override;

    /** Called for each event.
      This is where you do the real work.
    */
    int process_event(PHCompositeNode *topNode) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    int process_g4hits_inner(PHCompositeNode *, const std::string&);
    int process_g4hits_outer(PHCompositeNode *, const std::string&);

    // Analyze the hits
    int analysis(PHCompositeNode *);


    // private:
    protected:

        std::string detector;
        std::string outfilename;
        Fun4AllHistoManager *hm;

        TFile *outfile;

        TNtuple *g4hitntuple_inner;
        TNtuple *g4hitntuple_outer;

        TH1F *x_inner;
        TH1F *x_outer;
        TH1F *x_diff;

        double xa_inner;
        double xa_outer;
        double xa_diff;

        TH2F* h2_VD_XY; 
        TH2F* h2_VD_pXpY; 

};


#endif // MAGNETMATERIALANALYSIS_H
