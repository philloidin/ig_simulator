#pragma once

#include "../include_me.hpp"
#include "../gene_database.hpp"
#include "removing_settings.hpp"
#include "p_insertion_settings.hpp"
#include "n_insertion_settings.hpp"

// ----------------------------------------------------------------------------

class HC_VDJ_Recombination {
    const HC_GenesDatabase &db_;
    size_t vgene_index_;
    size_t dgene_index_;
    size_t jgene_index_;

    // endonuclease removing settings
    HC_RemovingSettings removing_settings_;

    // palindromic settings
    HC_PInsertionSettings p_insertion_settings_;

    // n-nucleotides insertion
    HC_NInsertionSettings n_insertion_settings_;

    // sequence and update flag
    string sequence_;
    bool update_sequence_;

    string ComputeSequence() const {
        return VgeneSeq().substr(0, VgeneLen()) +
                p_insertion_settings_.VEnd() + n_insertion_settings_.VD_Insertion() + p_insertion_settings_.DStart() +
                DgeneSeq().substr(removing_settings_.DStartLen(), DgeneLen()) +
                p_insertion_settings_.DEnd() + n_insertion_settings_.DJ_Insertion() + p_insertion_settings_.JStart() +
                JgeneSeq().substr(removing_settings_.JStartLen(), JgeneLen());
    }

public:
    HC_VDJ_Recombination(const HC_GenesDatabase &db,
            size_t vgene_index,
            size_t dgene_index,
            size_t jgene_index) :
            db_(db),
            vgene_index_(vgene_index),
            dgene_index_(dgene_index),
            jgene_index_(jgene_index),
            update_sequence_(true) {
    }

    size_t VgeneIndex() const { return vgene_index_; }

    size_t DgeneIndex() const { return dgene_index_; }

    size_t JgeneIndex() const { return jgene_index_; }

    // V gene block
    string VgeneName() const {
        return db_.GetByIndex(variable_gene, vgene_index_).GeneName();
    }

    string VgeneSeq() const {
        return db_.GetByIndex(variable_gene, vgene_index_).GeneSeq();
    }

    size_t VgeneLen() const {
        return VgeneSeq().size() - removing_settings_.VEndLen();
    }

    // D gene block
    string DgeneName() const {
        return db_.GetByIndex(diversity_gene, dgene_index_).GeneName();
    }

    string DgeneSeq() const {
        return db_.GetByIndex(diversity_gene, dgene_index_).GeneSeq();
    }

    size_t DgeneLen() const {
        return DgeneSeq().size() - removing_settings_.DStartLen() - removing_settings_.DEndLen();
    }

    // D gene block
    string JgeneName() const {
        return db_.GetByIndex(join_gene, jgene_index_).GeneName();
    }

    string JgeneSeq() const {
        return db_.GetByIndex(join_gene, jgene_index_).GeneSeq();
    }

    size_t JgeneLen() const {
        return JgeneSeq().size() - removing_settings_.JStartLen();
    }

    // settings get/set methods
    void AddRemovingSettings(HC_RemovingSettings removing_settings) {
        removing_settings_ = removing_settings;
        update_sequence_ = true;
    }

    const HC_RemovingSettings RemovingSettings() const { return removing_settings_; }

    void AddPInsertionSettings(HC_PInsertionSettings p_insertion_settings) {
        p_insertion_settings_ = p_insertion_settings;
        update_sequence_ = true;
    }

    const HC_PInsertionSettings PInsertionSettings() const { return p_insertion_settings_; }

    void AddNInsertionSettings(HC_NInsertionSettings n_insertion_settings) {
       n_insertion_settings_ = n_insertion_settings;
       update_sequence_ = true;
    }

    const HC_NInsertionSettings NInsertionSettings() const { return n_insertion_settings_; }

    const HC_GenesDatabase& GeneDB() const { return db_; }

    string Sequence() {
        if(update_sequence_)
            sequence_ = ComputeSequence();
        update_sequence_ = false;
        return sequence_;
    }
};

ostream& operator<<(ostream &out, HC_VDJ_Recombination &obj) {
    out << "Indices: " << obj.VgeneIndex() << " " << obj.DgeneIndex() << " " << obj.JgeneIndex() << endl;
    out << "Vgene. " << obj.VgeneName() << endl;
    out << "Dgene. " << obj.DgeneName() << endl;
    out << "Jgene. " << obj.JgeneName() << endl;
    out << "Endonuclease removals:" << endl;
    out << obj.RemovingSettings();
    out << "P nucleotides settings:" << endl;
    out << obj.PInsertionSettings();
    out << "N nucleotides settings:" << endl;
    out << obj.NInsertionSettings();
    out << "Sequence: " << obj.Sequence() << endl;
    return out;
}

typedef shared_ptr<HC_VDJ_Recombination> HC_VDJ_Recombination_Ptr;

// ----------------------------------------------------------------------------

class LC_VDJ_Recombination {
    const LC_GenesDatabase &db_;
    size_t vgene_index_;
    size_t jgene_index_;

    // endonuclease removing settings
    LC_RemovingSettings removing_settings_;

    // palindromic settings
    LC_PInsertionSettings p_insertion_settings_;

    // n-nucleotides insertion
    LC_NInsertionSettings n_insertion_settings_;

public:
    LC_VDJ_Recombination(const LC_GenesDatabase &db,
            size_t vgene_index,
            size_t jgene_index) :
            db_(db),
            vgene_index_(vgene_index),
            jgene_index_(jgene_index) { }

    size_t VgeneIndex() const { return vgene_index_; }

    size_t JgeneIndex() const { return jgene_index_; }

    string VgeneName() const {
        return db_.GetByIndex(variable_gene, vgene_index_).GeneName();
    }

    string JgeneName() const {
        return db_.GetByIndex(join_gene, jgene_index_).GeneName();
    }

    void AddRemovingSettings(LC_RemovingSettings removing_settings) {
        removing_settings_ = removing_settings;
    }

    const LC_RemovingSettings RemovingSettings() const { return removing_settings_; }

    void AddPInsertionSettings(LC_PInsertionSettings p_insertion_settings) {
        p_insertion_settings_ = p_insertion_settings;
    }

    const LC_PInsertionSettings PInsertionSettings() const { return p_insertion_settings_; }

    void AddNInsertionSettings(LC_NInsertionSettings n_insertion_settings) {
        n_insertion_settings_ = n_insertion_settings;
    }

    const LC_NInsertionSettings NInsertionSettings() const { return n_insertion_settings_; }

    const LC_GenesDatabase& GeneDB() const { return db_; }
};

ostream& operator<<(ostream &out, const LC_VDJ_Recombination &obj) {
    out << "Indices: " << obj.VgeneIndex() << " " << obj.JgeneIndex() << endl;
    out << "Vgene. " << obj.VgeneName() << endl;
    out << "Jgene. " << obj.JgeneName() << endl;
    out << "Endonuclease removals:" << endl;
    out << obj.RemovingSettings();
    out << "P nucleotides settings:" << endl;
    out << obj.PInsertionSettings();
    out << "N nucleotides settings:" << endl;
    out << obj.NInsertionSettings();
    return out;
}

typedef shared_ptr<LC_VDJ_Recombination> LC_VDJ_Recombination_Ptr;