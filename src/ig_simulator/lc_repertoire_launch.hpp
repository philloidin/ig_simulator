#include "gene_database.hpp"
#include "base_repertoire_creation/vdj_recombinator.hpp"
#include "base_repertoire_creation/exonuclease_remover.hpp"
#include "base_repertoire_creation/p_nucleotides_creator.hpp"
#include "base_repertoire_creation/n_nucleotides_creator.hpp"
#include "base_repertoire_creation/cdr_labeler.hpp"
#include "repertoire.hpp"
#include "multiplicity_creator.hpp"
#include "mutated_repertoire_creation/shm_creator.hpp"

LC_GenesDatabase_Ptr CreateLCDatabase(LC_InputParams params) {
    LC_GenesDatabase_Ptr lc_database(new LC_GenesDatabase());
    lc_database->AddGenesFromFile(variable_gene, params.vgenes_fname);
    lc_database->AddGenesFromFile(join_gene, params.jgenes_fname);
    return lc_database;
}

LC_Repertoire_Ptr CreateBaseLCRepertoire(LC_InputParams params, LC_GenesDatabase_Ptr lc_database) {
    LC_SimpleRecombinator lc_vdj_recombinator(lc_database, LC_SimpleRecombinationCreator(lc_database));

    // endonuclease remover
    LC_SimpleRemovingStrategy removing_strategy;
    LC_SimpleExonucleaseRemover remover(removing_strategy);

    // p nucleotides insertion
    LC_SimplePInsertionStrategy p_nucls_strategy;
    LC_SimplePNucleotidesCreator p_creator(p_nucls_strategy);

    // n nucleotides insertion
    LC_SimpleNInsertionStrategy n_nucls_strategy;
    LC_SimpleNNucleotidesCreator n_creator(n_nucls_strategy);

    // repertoire
    LC_Repertoire_Ptr base_repertoire(new LC_Repertoire());

    // base multiplicity creator
    double base_lambda = double(params.basic_repertoire_params.base_repertoire_size) /
            params.basic_repertoire_params.mutated_repertoire_size;
    LC_ExponentialMultiplicityCreator base_multiplicity_creator(base_lambda);

    // cdr labeling
    LC_CDRLabelingStrategy cdr_labeling_strategy;
    LC_CDRLabeler cdr_labeler(cdr_labeling_strategy);

    for(size_t i = 0; i < params.basic_repertoire_params.base_repertoire_size; i++) {
        auto vdj = lc_vdj_recombinator.CreateRecombination();
        vdj = remover.CreateRemovingSettings(vdj);
        vdj = p_creator.CreatePNucleotides(vdj);
        vdj = n_creator.CreateNNucleotides(vdj);

        LC_VariableRegionPtr ig_variable_region = LC_VariableRegionPtr(new LC_VariableRegion(vdj));
        ig_variable_region = cdr_labeler.LabelCDRs(ig_variable_region);

        size_t multiplicity = base_multiplicity_creator.AssignMultiplicity(ig_variable_region);
        base_repertoire->Add(LC_Cluster(ig_variable_region, multiplicity));
    }
    return base_repertoire;
}

void PrintBaseLCRepertoire(LC_Repertoire_Ptr base_repertoire) {
    for(auto it = base_repertoire->begin(); it != base_repertoire->end(); it++) {
        cout << it->IgVariableRegion()->VDJ_Recombination()->Sequence() << " " << it->Multiplicity() << endl;
        cout << it->IgVariableRegion()->GetCDRSettings();
    }
}

LC_Repertoire_Ptr CreateMutatedLCRepertoire(LC_InputParams params, LC_Repertoire_Ptr base_repertoire) {
    LC_Repertoire_Ptr mutated_repertoire(new LC_Repertoire());

    // mutated multiplicity creator
    double mutated_lambda = double(base_repertoire->NumberAntibodies()) /
            double(params.basic_repertoire_params.final_repertoire_size);
    LC_ExponentialMultiplicityCreator mutated_multiplicity_creator(mutated_lambda);

    // shm creator
    LC_RgywWrcySHMStrategy shm_creation_strategy1(params.pattern_shm_params.min_number_pattern_shm,
                                                  params.pattern_shm_params.max_number_pattern_shm,
                                                  params.pattern_shm_params.substitution_propability);
    LC_CDRBasedRandomSHMStrategy shm_creation_strategy2(params.cdr_shm_params.min_number_mutations,
                                                        params.cdr_shm_params.max_number_mutations,
                                                        params.cdr_shm_params.mutation_in_fr_prop);
    LC_CompositeSHMCreationStrategy composite_shm_strategy(shm_creation_strategy1, shm_creation_strategy2);
    LC_SHMCreator shm_creator(composite_shm_strategy);

    for(auto it = base_repertoire->begin(); it != base_repertoire->end(); it++) {
        for(size_t i = 0; i < it->Multiplicity(); i++) {
            auto variable_region_ptr = it->IgVariableRegion()->Clone();
            variable_region_ptr = shm_creator.CreateSHM(variable_region_ptr);
            size_t multiplicity = mutated_multiplicity_creator.AssignMultiplicity(variable_region_ptr);
            mutated_repertoire->Add(LC_Cluster(variable_region_ptr, multiplicity));
        }
    }
    return mutated_repertoire;
}

void CreateLCRepertoire(LC_InputParams params) {
    cout << "======== Simulation of light chain repertoire starts" << endl;
    cout << "Repertoire parameters: " << endl;
    cout << params.basic_repertoire_params << endl;

    cout << "Database parameters: " << endl;
    params.PrintDatabaseParams();
    cout << endl;

    auto lc_database = CreateLCDatabase(params);

    cout << "==== Generation of base repertoire" << endl;
    LC_Repertoire_Ptr base_repertoire = CreateBaseLCRepertoire(params, lc_database);
    cout << "Base repertoire consists of " << base_repertoire->Size() << " sequences with total multiplicities " <<
    base_repertoire->NumberAntibodies() << endl << endl;

    cout << "==== Generation of mutated repertoire" << endl;
    LC_Repertoire_Ptr mutated_repertoire = CreateMutatedLCRepertoire(params, base_repertoire);
    cout << "Mutated repertoire consists of " << mutated_repertoire->Size() <<
        " sequences with total multiplicities " << mutated_repertoire->NumberAntibodies() << endl << endl;

    cout << "======== Simulation of heavy chain repertoire ends" << endl;
}