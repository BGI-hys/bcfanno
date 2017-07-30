
BCFANNO manual
==========


## Release note:

*release v1.0 :  first stable version, renamed vcfanno to bcfanno.*

## **Introduction**

bcfanno was designed to annotate VCF/BCF files by using local and online databases. bcfanno support three kinds of databases which list below. And users could download and build open source or in house databases manually. Please refer to **database** section for the details.

Borrow the advances of VCF format, bcfanno was designed to put all the annotated data (dubbed *tags*) in the *INFO*, and the annotated VCF file could be convert to other formats or reused as a database. So before you try to use bcfanno, please make sure you know the structure of VCF file and how to find the document of [VCF/BCF format](https://samtools.github.io/hts-specs/VCFv4.2.pdf).


## Bechmark

bcfanno annotate a WGS vcf with specified 22 databases in 6 CPU hours. bcfanno usually do not parse the FORMAT of vcf files, and all the annotators (*tags*) will be put into the INFO region, so it is suggested to merge multiple samples by `bcftools merge` before annotation.



## **How to build the programs**

*prerequisite:*

* zlib

```
git clone https://github.com/shiquan/vcfanno.git
cd vcfanno
make
```

Following execute programs should be compiled after several miniters.

* ***vcfanno*** , core program to annotate genetic variants
* [***tsv2vcf***](https://github.com/shiquan/vcfanno/blob/master/documents/tsv2vcf_manual.md) ,  generate VCF databases from tab-seperated file
* [***vcf2tsv***](https://github.com/shiquan/vcfanno/blob/master/documents/vcf2tsv_manual.md), convert VCF file to tab-separated file with selected tags
* [***vcf_rename_tags***](https://github.com/shiquan/vcfanno/blob/master/documents/vcf_rename_tags_manual.md), rename tags or contig names in the VCF file, usually used to format the databases

*Some other programs should also be install for ongoing test.*

* [bcftools](http://www.htslib.org/download/) 
* [tabix](http://www.htslib.org/download/) (tabix is now a part of HTSlib, so download the htslib and complier it, and you will find tabix then)


## **Databases**

bcfanno support THREE kinds of databases,

* *allele specific databases*, like allele frequency or any other allele related databases;
* *region specific databases*, like gene region, function regions or any other genome region related databases;
* *transcript specific databases*, like protein id or any other transcript related databases.

I think most of database could be classsified into above three kinds. Please email if these three don't define your databases properly.

Before you try to use any database, please try to classify the data and follow the below instruction to check or convert the database for bcfanno.


## **Generate databases for bcfanno**

Databases should be convert to VCF/BCF and BED-like region format. Good thing is the most databases were released in VCF or BED-like format, so we just need download them and do some updates for these kind of files, like dbsnp, EXAC and ClinVar etc. However, there are still some databases like dbNSFP were released in plain text format or other format, and we should convert them manually. For this section, we are only trying to build *clinvar* for getting start. All the details about build and convert databases could be find at [More details about databases](https://github.com/shiquan/vcfanno/blob/master/documents/database/more_details.md).


*Instruction to build dbsnp for bcfanno:*

Step 1, enter the example directory and download clinvar in it.

`wget -c ftp://ftp.ncbi.nlm.nih.gov/pub/clinvar/vcf_GRCh37/clinvar.vcf.gz`

Step 2, build index for clinvar.

`bcftools index clinvar.vcf.gz`

Step 3, write configure file for bcfanno.

Create *clinvar.json*, and this file should be looked like

```
{
        "vcfs":[
        	{
                	"file":"path to clinvar.vcf.gz",
                        "columns":"RS,CLNSIG",
                },
         ],
}
```

Step 4, annotation. (If you are in the example directory now you can just run this command)

`../vcfanno -c clinvar.json demo.vcf`

The demo.vcf file would be annotated with clinvar databases. Try to compare the raw vcf and annotated vcf, see what was happened.

```
$ cat example/demo.vcf
##fileformat=VCFv4.2
##reference=file://17.fa.gz
##contig=<ID=17,length=81195210>
##ALT=<ID=X,Description="Represents allele(s) other than observed.">
##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
#CHROM	POS	ID	REF	ALT	QUAL	FILTER	INFO	FORMAT	demo
17	41222826	.	A	C	.	.	.	GT	0/1
17	41223242	.	G	C	.	.	.	GT	0/1
17	41234451	.	G	A	.	.	.	GT	0/1
17	41258326	.	A	G	.	.	.	GT	0/1
17	41258504	.	A	C	.	.	.	GT	0/1
```

After annotation.

```
$ cat example/demo.vcf
##fileformat=VCFv4.2
##reference=file://17.fa.gz
##contig=<ID=17,length=81195210>
##ALT=<ID=X,Description="Represents allele(s) other than observed.">
##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
#CHROM	POS	ID	REF	ALT	QUAL	FILTER	INFO	FORMAT	demo
17	41222826	.	A	C	.	.	.	GT	0/1
17	41223242	.	G	C	.	.	.	GT	0/1
17	41234451	.	G	A	.	.	.	GT	0/1
17	41258326	.	A	G	.	.	.	GT	0/1
17	41258504	.	A	C	.	.	.	GT	0/1

$ cat example/demo_anno.vcf 
##fileformat=VCFv4.2
##FILTER=<ID=PASS,Description="All filters passed">
##reference=file://17.fa.gz
##contig=<ID=17,length=81195210>
##ALT=<ID=X,Description="Represents allele(s) other than observed.">
##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
##INFO=<ID=Gene,Number=A,Type=String,Description="Gene names">
##INFO=<ID=Transcript,Number=A,Type=String,Description="Transcript names">
##INFO=<ID=HGVSnom,Number=A,Type=String,Description="HGVS nomenclature for the description of DNA sequence variants">
##INFO=<ID=IVSnom,Number=A,Type=String,Description="Old style nomenclature for the description of intron variants. Not recommand to use it in normal practice.">
##INFO=<ID=Oldnom,Number=A,Type=String,Description="Old style nomenclature for the description of variants. Based on the gene position. Compared with HGVS nomenclature, Oldnom also count UTR5 length for coding transcript. For noncoding transcript, HGVSnom is same with Oldnom.">
##INFO=<ID=AAlength,Number=A,Type=String,Description="Amino acid length for each transcript. For noncoding transcript AAlength should always be 0.">
##INFO=<ID=ExonIntron,Number=A,Type=String,Description="Exon/CDS or intron id on transcripts.">
##INFO=<ID=VarType,Number=A,Type=String,Description="Variant type.">
##INFO=<ID=EXAC_AF_AFR,Number=A,Type=Float,Description="African/African American Allele Frequency, =EXAC_AC_AFR/EXAC_AN_AFR">
##INFO=<ID=EXAC_AF_AMR,Number=A,Type=Float,Description="American Allele Frequency, =EXAC_AC_AMR/EXAC_AN_AMR">
##INFO=<ID=EXAC_AF_EAS,Number=A,Type=Float,Description="East Asian Allele Frequency, =EXAC_AC_EAS/EXAC_AN_EAS">
##INFO=<ID=NBWGS_AC,Number=A,Type=Integer,Description="Counts of each allele for all samples.">
##INFO=<ID=NBWGS_AF,Number=A,Type=Float,Description="Frequency of each allele for all samples.">
##INFO=<ID=ESP6500_MAF,Number=.,Type=String,Description="Minor Allele Frequency in percent in the order of EA,AA,All">
##INFO=<ID=DBSNP_ASP,Number=0,Type=Flag,Description="Is Assembly specific. This is set if the variant only maps to one assembly">
##INFO=<ID=DBSNP_ASS,Number=0,Type=Flag,Description="In acceptor splice site FxnCode = 73">
##INFO=<ID=DBSNP_CAF,Number=.,Type=String,Description="An ordered, comma delimited list of allele frequencies based on 1000Genomes, starting with the reference allele followed by alternate alleles as ordered in the ALT column. Where a 1000Genomes alternate allele is not in the dbSNPs alternate allele set, the allele is added to the ALT column.  The minor allele is the second largest value in the list, and was previuosly reported in VCF as the GMAF.  This is the GMAF reported on the RefSNP and EntrezSNP pages and VariationReporter">
##INFO=<ID=DBSNP_CFL,Number=0,Type=Flag,Description="Has Assembly conflict. This is for weight 1 and 2 variant that maps to different chromosomes on different assemblies.">
##INFO=<ID=CLNDBN,Number=.,Type=String,Description="Variant disease name">
##INFO=<ID=CLNDSDB,Number=.,Type=String,Description="Variant disease database name">
##INFO=<ID=CLNORIGIN,Number=.,Type=String,Description="Allele Origin. One or more of the following values may be added: 0 - unknown; 1 - germline; 2 - somatic; 4 - inherited; 8 - paternal; 16 - maternal; 32 - de-novo; 64 - biparental; 128 - uniparental; 256 - not-tested; 512 - tested-inconclusive; 1073741824 - other">
##INFO=<ID=CLNSIG,Number=.,Type=String,Description="Variant Clinical Significance, 0 - unknown, 1 - untested, 2 - non-pathogenic, 3 - probable-non-pathogenic, 4 - probable-pathogenic, 5 - pathogenic, 6 - drug-response, 7 - histocompatibility, 255 - other">
##INFO=<ID=DBSNP_COMMON,Number=1,Type=Integer,Description="RS is a common SNP.  A common SNP is one that has at least one 1000Genomes population with a minor allele of frequency >= 1% and for which 2 or more founders contribute to that minor allele frequency.">
##INFO=<ID=DBSNP_DSS,Number=0,Type=Flag,Description="In donor splice-site FxnCode = 75">
##INFO=<ID=RS,Number=1,Type=Integer,Description="dbSNP ID (i.e. rs number)">
##INFO=<ID=DBSNP_SAO,Number=1,Type=Integer,Description="Variant Allele Origin: 0 - unspecified, 1 - Germline, 2 - Somatic, 3 - Both">
##INFO=<ID=DBSNP_VLD,Number=0,Type=Flag,Description="Is Validated.  This bit is set if the variant has 2+ minor allele count based on frequency or genotype data.">
##INFO=<ID=DBSNP_VP,Number=1,Type=String,Description="Variation Property.  Documentation is at ftp://ftp.ncbi.nlm.nih.gov/snp/specs/dbSNP_BitField_latest.pdf">
##INFO=<ID=CLINVAR_VP,Number=1,Type=String,Description="Variation Property.  Documentation is at ftp://ftp.ncbi.nlm.nih.gov/snp/specs/dbSNP_BitField_latest.pdf">
##INFO=<ID=CLINVAR_CLNSIG,Number=.,Type=String,Description="Variant Clinical Significance, 0 - Uncertain significance, 1 - not provided, 2 - Benign, 3 - Likely benign, 4 - Likely pathogenic, 5 - Pathogenic, 6 - drug response, 7 - histocompatibility, 255 - other">
##INFO=<ID=CLINVAR_CLNDBN,Number=.,Type=String,Description="Variant disease name">
##INFO=<ID=G1000_AF,Number=A,Type=Float,Description="Estimated allele frequency in the range (0,1)">
##INFO=<ID=G1000_EAS_AF,Number=A,Type=Float,Description="Allele frequency in the EAS populations calculated from AC and AN, in the range (0,1)">
##INFO=<ID=G1000_EUR_AF,Number=A,Type=Float,Description="Allele frequency in the EUR populations calculated from AC and AN, in the range (0,1)">
##INFO=<ID=G1000_AFR_AF,Number=A,Type=Float,Description="Allele frequency in the AFR populations calculated from AC and AN, in the range (0,1)">
##INFO=<ID=G1000_AMR_AF,Number=A,Type=Float,Description="Allele frequency in the AMR populations calculated from AC and AN, in the range (0,1)">
##INFO=<ID=G1000_SAS_AF,Number=A,Type=Float,Description="Allele frequency in the SAS populations calculated from AC and AN, in the range (0,1)">
##INFO=<ID=dbscSNV_ADA_SCORE,Number=A,Type=Float,Description="Ensemble prediction score based on ada-boost. Ranges 0 to 1. The larger the score the higher probability the scSNV will affect splicing. The suggested cutoff for a binary prediction (affecting splicing vs. not affecting splicing) is 0.6">
##INFO=<ID=dbscSNV_RF_SCORE,Number=A,Type=Float,Description="Ensemble prediction score based on random forests. Ranges 0 to 1. The larger the score the higher probability the scSNV will affect splicing. The suggested cutoff for a binary prediction (affecting splicing vs. not affecting splicing) is 0.6">
##INFO=<ID=HGMD_disease,Number=A,Type=String,Description="HGMD disease.">
##INFO=<ID=HGMD_descr,Number=A,Type=String,Description="HGMD description.">
##INFO=<ID=HGMD_tag,Number=A,Type=String,Description="HGMD tag, 'DP','FP','DFP','DM','DM?','FTV','R'.">
##INFO=<ID=HGMD_pmid,Number=A,Type=String,Description="HGMD pmid.">
##INFO=<ID=HGMD_acc_num,Number=A,Type=String,Description="HGMD acc_num.">
##INFO=<ID=HGMD_base,Number=,Type=String,Description="HGMD base.">
##INFO=<ID=SIFT_score,Number=A,Type=String,Description="SIFT score (SIFTori). Scores range from 0 to 1. The smaller the score the more likely the SNP has damaging effect. Multiple scores separated by |, corresponding to Ensembl_proteinid.">
##INFO=<ID=Polyphen2_HDIV_score,Number=A,Type=String,Description="Polyphen2 score based on HumDiv, i.e. hdiv_prob. The score ranges from 0 to 1. Multiple entries separated by |">
##INFO=<ID=Polyphen2_HVAR_score,Number=A,Type=String,Description="Polyphen2 score based on HumVar, i.e. hvar_prob. The score ranges from 0 to 1. Multiple entries separated by |.">
##INFO=<ID=LRT_score,Number=A,Type=String,Description="The original LRT two-sided p-value (LRTori), ranges from 0 to 1.">
##INFO=<ID=LRT_pred,Number=A,Type=String,Description="LRT prediction, D(eleterious), N(eutral) or U(nknown), which is not solely determined by the score. ">
##INFO=<ID=MutationTaster_score,Number=A,Type=String,Description="MutationTaster p-value (MTori), ranges from 0 to 1.">
##INFO=<ID=MutationTaster_pred,Number=A,Type=String,Description="MutationTaster prediction, A (disease_causing_automatic), D (disease_causing), N (polymorphism) or P (polymorphism_automatic). The score cutoff between D and N is 0.5 for MTori and 0.31655 for the rankscore.">
##INFO=<ID=MutationAssessor_pred,Number=A,Type=String,Description="MutationAssessor's functional impact of a variant : predicted functional, i.e. high (H) or medium (M), or predicted non-functional, i.e. low (L) or neutral (N). The MAori score cutoffs between H and M, M and L, and L and N, are 3.5, 1.935 and 0.8, respectively. The rankscore cutoffs between H and M, M and L, and L and N, are 0.92924, 0.51945 and 0.19692, respectively.">
##INFO=<ID=FATHMM_pred,Number=A,Type=String,Description="If a FATHMMori score is <=-1.5 (or rankscore <=0.81415) the corresponding NS is predicted as D(AMAGING); otherwise it is predicted as T(OLERATED). Multiple predictions separated by |">
##INFO=<ID=PROVEAN_pred,Number=A,Type=String,Description="If PROVEANori <= -2.5 (rankscore>=0.59) the corresponding NS is predicted as D(amaging); otherwise it is predicted as N(eutral). Multiple predictions separated by |">
##INFO=<ID=CADD_raw,Number=A,Type=String,Description="CADD raw score for funtional prediction of a SNP. Please refer to Kircher et al. (2014) Nature Genetics 46(3):310-5 for details. The larger the score the more likely the SNP has damaging effect. Please note the following copyright statement for CADD: CADD scores (http://cadd.gs.washington.edu/) are Copyright 2013 University of Washington and Hudson-Alpha Institute for Biotechnology (all rights reserved) but are freely available for all academic, non-commercial applications. For commercial licensing information contact Jennifer McCullar (mccullaj@uw.edu).">
##INFO=<ID=fathmm-MKL_coding_score,Number=A,Type=String,Description="FATHMM default score (weighted for human inherited-disease mutations with Disease Ontology) (FATHMMori). Scores range from -18.09 to 11.0. Multiple scores separated by | Please refer to Shihab et al. (2013) Human Mutation 34(1):57-65 for details.">
##INFO=<ID=MetaSVM_score,Number=A,Type=String,Description="Our support vector machine (SVM) based ensemble prediction score, which incorporated 10 scores (SIFT, PolyPhen-2 HDIV, PolyPhen-2 HVAR, GERP++, MutationTaster, Mutation Assessor, FATHMM, LRT, SiPhy, PhyloP) and the maximum frequency observed in the 1000 genomes populations. Larger value means the SNV is more likely to be damaging. Scores range from -2 to 3 in dbNSFP.">
##INFO=<ID=MetaLR_score,Number=A,Type=String,Description="Our logistic regression (LR) based ensemble prediction score, which incorporated 10 scores (SIFT, PolyPhen-2 HDIV, PolyPhen-2 HVAR, GERP++, MutationTaster, Mutation Assessor, FATHMM, LRT, SiPhy, PhyloP) and the maximum frequency observed in the 1000 genomes populations. Larger value means the SNV is more likely to be damaging. Scores range from 0 to 1.">
##INFO=<ID=GERP++_RS,Number=A,Type=String,Description="GERP++ RS score, the larger the score, the more conserved the site.">
##INFO=<ID=SiPhy_29way_logOdds,Number=A,Type=String,Description="SiPhy score based on 29 mammals genomes. The larger the score, the more conserved the site.">
##INFO=<ID=Gnomad_AF,Number=A,Type=Float,Description="Allele Frequency among genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_AFR,Number=A,Type=Float,Description="Allele Frequency among African/African American genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_AMR,Number=A,Type=Float,Description="Allele Frequency among Admixed American genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_ASJ,Number=A,Type=Float,Description="Allele Frequency among Ashkenazi Jewish genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_EAS,Number=A,Type=Float,Description="Allele Frequency among East Asian genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_FIN,Number=A,Type=Float,Description="Allele Frequency among Finnish genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_NFE,Number=A,Type=Float,Description="Allele Frequency among Non-Finnish European genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_OTH,Number=A,Type=Float,Description="Allele Frequency among Other (population not assigned) genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=gnomad_exomes_AF_SAS,Number=A,Type=Float,Description="Allele Frequency among South Asian genotypes, for each ALT allele, in the same order as listed">
##INFO=<ID=mcap,Number=A,Type=Float,Description="MCAP score">
##INFO=<ID=revel,Number=A,Type=Float,Description="MCAP score">
##INFO=<ID=CytoBand,Number=1,Type=String,Description="Name of cytogenetic band">
##INFO=<ID=CGD_CONDITION,Number=1,Type=String,Description="CONDITION">
##INFO=<ID=CGD_INHERITANCE,Number=1,Type=String,Description="INHERITANCE">
##INFO=<ID=Gene_MIM_id,Number=1,Type=String,Description="Gene MIM id">
##INFO=<ID=Related_disorder_MIM_id,Number=1,Type=String,Description="Related disorder MIM id">
##INFO=<ID=Possible_Inheritance,Number=1,Type=String,Description="Possible Inheritance">
##INFO=<ID=GWAS_trait,Number=1,Type=String,Description="Disease or trait examined in study">
##INFO=<ID=TrfSeq,Number=1,Type=String,Description="Tandem Repeats Finder locations, filtered to keep repeats with period less than or equal to 12">
##INFO=<ID=Haplo_Score,Number=1,Type=String,Description="ClinGen Gene Curation Haplosensitivity Scores">
##INFO=<ID=Triplo_Score,Number=1,Type=String,Description="ClinGen Gene Curation Triplosensitivity Scores">
##vcfannoVersion=v0.0.2-62-g285f0a6+htslib-1.3
##vcfannoCommand=-c /ifs5/ST_TRANS_CARDIO/PUB/vcfanno_databases/hg19/newborn_anno_config_v1.json
#CHROM	POS	ID	REF	ALT	QUAL	FILTER	INFO	FORMAT	demo
17	41222826	.	A	C	.	.	Gene=BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1;Transcript=NM_007294.3|NM_007297.3|NM_007298.3|NM_007299.3|NM_007300.3|NR_027676.1;HGVSnom=NM_007294.3:c.4987-3114A>C|NM_007297.3:c.4846-3114A>C|NM_007298.3:c.1675-3114A>C|NM_007299.3:c.1675-3114A>C|NM_007300.3:c.5050-3114A>C|NR_027676.1:n.5123-3114A>C;VarType=intron|intron|intron|intron|intron|intron;IVSnom=NM_007294.3:IVS15-3114A>C|NM_007297.3:IVS14-3114A>C|NM_007298.3:IVS14-3114A>C|NM_007299.3:IVS15-3114A>C|NM_007300.3:IVS16-3114A>C|NR_027676.1:IVS15-3114A>C;Oldnom=NM_007294.3:n.5219-3114A>C|NM_007297.3:n.5127-3114A>C|NM_007298.3:n.1694-3114A>C|NM_007299.3:n.1869-3114A>C|NM_007300.3:n.5282-3114A>C;AAlength=1864|1817|760|700|1885|0;ExonIntron=I15|I14|I14|I15|I16|I15;G1000_AF=0.000199681;G1000_EAS_AF=0;G1000_EUR_AF=0;G1000_AFR_AF=0.0008;G1000_AMR_AF=0;G1000_SAS_AF=0;Gnomad_AF=3.22789e-05;CytoBand=q21.31;CGD_CONDITION=Osteopetrosis,autosomal recessive 6,Koolen-de Vries syndrom,Supranuclear palsy,progressive|Frontotemporal dementia|Parkinson-dementia syndrome|Pick disease,Breast-ovarian cancer,familial,susceptibility to,1|Pancreatic cancer,susceptibility to,4;CGD_INHERITANCE=AR,AD,AD/AR;Gene_MIM_id=113705;Related_disorder_MIM_id=604370(SK)|614320(SK);Possible_Inheritance=AD;Haplo_Score=3;Triplo_Score=0	GT	0/1
17	41223242	.	G	C	.	.	Gene=BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1;Transcript=NM_007294.3|NM_007297.3|NM_007298.3|NM_007299.3|NM_007300.3|NR_027676.1;HGVSnom=NM_007294.3:c.4689C>G(p.Tyr1563Stop/p.Y1563X)|NM_007297.3:c.4548C>G(p.Tyr1516Stop/p.Y1516X)|NM_007298.3:c.1377C>G(p.Tyr459Stop/p.Y459X)|NM_007299.3:c.1377C>G(p.Tyr459Stop/p.Y459X)|NM_007300.3:c.4752C>G(p.Tyr1584Stop/p.Y1584X)|NR_027676.1:n.4825C>G;VarType=nonsense|nonsense|nonsense|nonsense|nonsense|noncoding;Oldnom=NM_007294.3:n.4921C>G|NM_007297.3:n.4829C>G|NM_007298.3:n.1396C>G|NM_007299.3:n.1571C>G|NM_007300.3:n.4984C>G;AAlength=1864|1817|760|700|1885|0;ExonIntron=E15/C14|E14/C12|E14/C14|E15/C14|E16/C15|E15/C15;RS=80357433;DBSNP_SAO=0;DBSNP_VP=0x050160000601000002100100;CLINVAR_VP=0x050160000605000002100100;CLINVAR_CLNSIG=5|5|5|5;CLINVAR_CLNDBN=Breast-ovarian_cancer\x2c_familial_1|Hereditary_breast_and_ovarian_cancer_syndrome|Hereditary_cancer-predisposing_syndrome|not_provided;HGMD_disease=Breast cancer;HGMD_descr=Tyr1563Term;HGMD_tag=DM;HGMD_pmid=8554067;HGMD_acc_num=CM960185;HGMD_base=M;SIFT_score=.|.|.|.|.|.|.|.|.|.|.|.|.;LRT_score=0.822679;LRT_pred=N;MutationTaster_score=1|1|1|1|1|1|1|1|1|1|5.25423e-07|5.25423e-07|5.25423e-07|5.25423e-07;MutationTaster_pred=A|A|A|A|A|A|A|A|A|A|A|A|A|A;PROVEAN_pred=.|.|.|.|.|.|.|.|.|.|.|.|.;CADD_raw=8.634610;GERP++_RS=1.5;SiPhy_29way_logOdds=4.504;gnomad_exomes_AF_AFR=0;gnomad_exomes_AF_AMR=0;gnomad_exomes_AF_ASJ=0;gnomad_exomes_AF_EAS=0;gnomad_exomes_AF_FIN=0;gnomad_exomes_AF_NFE=1.80106e-05;gnomad_exomes_AF_OTH=0;gnomad_exomes_AF_SAS=0;CytoBand=q21.31;CGD_CONDITION=Osteopetrosis,autosomal recessive 6,Koolen-de Vries syndrom,Supranuclear palsy,progressive|Frontotemporal dementia|Parkinson-dementia syndrome|Pick disease,Breast-ovarian cancer,familial,susceptibility to,1|Pancreatic cancer,susceptibility to,4;CGD_INHERITANCE=AR,AD,AD/AR;Gene_MIM_id=113705;Related_disorder_MIM_id=604370(SK)|614320(SK);Possible_Inheritance=AD;Haplo_Score=3;Triplo_Score=0	GT	0/1
17	41234451	.	G	A	.	.	Gene=BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1;Transcript=NM_007294.3|NM_007297.3|NM_007298.3|NM_007299.3|NM_007300.3|NR_027676.1;HGVSnom=NM_007294.3:c.4327C>T(p.Arg1443Stop/p.R1443X)|NM_007297.3:c.4186C>T(p.Arg1396Stop/p.R1396X)|NM_007298.3:c.1018C>T(p.Arg340Stop/p.R340X)|NM_007299.3:c.1018C>T(p.Arg340Stop/p.R340X)|NM_007300.3:c.4327C>T(p.Arg1443Stop/p.R1443X)|NR_027676.1:n.4463C>T;VarType=nonsense|nonsense|nonsense|nonsense|nonsense|noncoding;Oldnom=NM_007294.3:n.4559C>T|NM_007297.3:n.4467C>T|NM_007298.3:n.1037C>T|NM_007299.3:n.1212C>T|NM_007300.3:n.4559C>T;AAlength=1864|1817|760|700|1885|0;ExonIntron=E12/C11|E11/C9|E11/C11|E12/C11|E12/C11|E12/C12;EXAC_AF_AFR=0;EXAC_AF_AMR=8.63707e-05;EXAC_AF_EAS=0;RS=41293455;DBSNP_SAO=1;DBSNP_VP=0x050160000e01000102110100;CLINVAR_VP=0x050168000a05000102110100;CLINVAR_CLNSIG=5|5|5|5|5|5,255|3|3;CLINVAR_CLNDBN=Breast-ovarian_cancer\x2c_familial_1|Hereditary_breast_and_ovarian_cancer_syndrome|Hereditary_cancer-predisposing_syndrome|Familial_cancer_of_breast|not_provided|not_specified,Breast-ovarian_cancer\x2c_familial_1|Hereditary_breast_and_ovarian_cancer_syndrome|Hereditary_cancer-predisposing_syndrome;HGMD_disease=Breast cancer;HGMD_descr=Arg1443Term;HGMD_tag=DM;HGMD_pmid=7894491;HGMD_acc_num=CM940180;HGMD_base=M;SIFT_score=.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.;LRT_score=0.532283;LRT_pred=N;MutationTaster_score=1|1|1|1|1|1|1|1|1|1|1|1|1|1;MutationTaster_pred=A|A|A|A|A|A|A|A|A|A|A|N|N|N;PROVEAN_pred=.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.;CADD_raw=0.163825;GERP++_RS=2.27;SiPhy_29way_logOdds=7.1166;Gnomad_AF=3.22893e-05;gnomad_exomes_AF_AFR=0;gnomad_exomes_AF_AMR=8.93549e-05;gnomad_exomes_AF_ASJ=0;gnomad_exomes_AF_EAS=0;gnomad_exomes_AF_FIN=4.48632e-05;gnomad_exomes_AF_NFE=1.79173e-05;gnomad_exomes_AF_OTH=0;gnomad_exomes_AF_SAS=0;CytoBand=q21.31;CGD_CONDITION=Osteopetrosis,autosomal recessive 6,Koolen-de Vries syndrom,Supranuclear palsy,progressive|Frontotemporal dementia|Parkinson-dementia syndrome|Pick disease,Breast-ovarian cancer,familial,susceptibility to,1|Pancreatic cancer,susceptibility to,4;CGD_INHERITANCE=AR,AD,AD/AR;Gene_MIM_id=113705;Related_disorder_MIM_id=604370(SK)|614320(SK);Possible_Inheritance=AD;Haplo_Score=3;Triplo_Score=0	GT	0/1
17	41258326	.	A	G	.	.	Gene=BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1;Transcript=NM_007294.3|NM_007297.3|NM_007298.3|NM_007299.3|NM_007300.3|NR_027676.1;HGVSnom=NM_007294.3:c.213-1353A>G|NM_007297.3:c.72-1353A>G|NM_007298.3:c.213-1353A>G|NM_007299.3:c.213-1353A>G|NM_007300.3:c.213-1353A>G|NR_027676.1:n.352-1353A>G;VarType=intron|intron|intron|intron|intron|intron;IVSnom=NM_007294.3:IVS4-1353A>G|NM_007297.3:IVS3-1353A>G|NM_007298.3:IVS3-1353A>G|NM_007299.3:IVS4-1353A>G|NM_007300.3:IVS4-1353A>G|NR_027676.1:IVS4-1353A>G;Oldnom=NM_007294.3:n.445-1353A>G|NM_007297.3:n.353-1353A>G|NM_007298.3:n.232-1353A>G|NM_007299.3:n.407-1353A>G|NM_007300.3:n.445-1353A>G;AAlength=1864|1817|760|700|1885|0;ExonIntron=I4|I3|I3|I4|I4|I4;CytoBand=q21.31;CGD_CONDITION=Osteopetrosis,autosomal recessive 6,Koolen-de Vries syndrom,Supranuclear palsy,progressive|Frontotemporal dementia|Parkinson-dementia syndrome|Pick disease,Breast-ovarian cancer,familial,susceptibility to,1|Pancreatic cancer,susceptibility to,4;CGD_INHERITANCE=AR,AD,AD/AR;Gene_MIM_id=113705;Related_disorder_MIM_id=604370(SK)|614320(SK);Possible_Inheritance=AD;Haplo_Score=3;Triplo_Score=0	GT	0/1
17	41258504	.	A	C	.	.	Gene=BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1;Transcript=NM_007294.3|NM_007297.3|NM_007298.3|NM_007299.3|NM_007300.3|NR_027676.1;HGVSnom=NM_007294.3:c.181T>G(p.Cys61Gly/p.C61G)|NM_007297.3:c.40T>G(p.Cys14Gly/p.C14G)|NM_007298.3:c.181T>G(p.Cys61Gly/p.C61G)|NM_007299.3:c.181T>G(p.Cys61Gly/p.C61G)|NM_007300.3:c.181T>G(p.Cys61Gly/p.C61G)|NR_027676.1:n.342T>G;VarType=missense|missense|missense|missense|missense|noncoding;Oldnom=NM_007294.3:n.413T>G|NM_007297.3:n.321T>G|NM_007298.3:n.200T>G|NM_007299.3:n.375T>G|NM_007300.3:n.413T>G;AAlength=1864|1817|760|700|1885|0;ExonIntron=E4/C3|E3/C1|E3/C3|E4/C3|E4/C3|E4/C4;EXAC_AF_AFR=0;EXAC_AF_AMR=0;EXAC_AF_EAS=0;RS=28897672;DBSNP_SAO=1;DBSNP_VP=0x050160000a01000502110100;CLINVAR_VP=0x050160000a05000502110104;CLINVAR_CLNSIG=5|5|5|5|4,1|0,1;CLINVAR_CLNDBN=Breast-ovarian_cancer\x2c_familial_1|Hereditary_breast_and_ovarian_cancer_syndrome|Hereditary_cancer-predisposing_syndrome|not_provided|Familial_cancer_of_breast,Familial_cancer_of_breast|Breast-ovarian_cancer\x2c_familial_1,Familial_cancer_of_breast;HGMD_disease=Breast cancer;HGMD_descr=Cys61Gly;HGMD_tag=DM;HGMD_pmid=7894493;HGMD_acc_num=CM940172;HGMD_base=M;SIFT_score=0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0;Polyphen2_HDIV_score=1.0|0.987|0.999|0.999|1.0|1.0|0.116;Polyphen2_HVAR_score=0.996|0.985|1.0|0.994|0.999|0.999|0.026;LRT_score=0.000044;LRT_pred=D;MutationTaster_score=0.999465|0.999465|0.992709|0.999347|0.992709|1|1|1|1|0.995386|0.995386|0.951368|0.952625|0.641453;MutationTaster_pred=D|D|D|D|D|D|D|D|D|N|N|N|N|N;MutationAssessor_pred=H;FATHMM_pred=D|D|D|D|D|D|D|D|D|D|D|D|D|D|D;PROVEAN_pred=N|N|N|N|N|N|D|N|N|N|D|D|D|N|D|D;CADD_raw=4.399001;MetaSVM_score=0.9506;MetaLR_score=0.9929;GERP++_RS=5.17;SiPhy_29way_logOdds=12.6756;gnomad_exomes_AF_AFR=0;gnomad_exomes_AF_AMR=0;gnomad_exomes_AF_ASJ=0;gnomad_exomes_AF_EAS=0;gnomad_exomes_AF_FIN=4.48833e-05;gnomad_exomes_AF_NFE=6.27735e-05;gnomad_exomes_AF_OTH=0;gnomad_exomes_AF_SAS=0;mcap=0.963103;revel=0.948;CytoBand=q21.31;CGD_CONDITION=Osteopetrosis,autosomal recessive 6,Koolen-de Vries syndrom,Supranuclear palsy,progressive|Frontotemporal dementia|Parkinson-dementia syndrome|Pick disease,Breast-ovarian cancer,familial,susceptibility to,1|Pancreatic cancer,susceptibility to,4;CGD_INHERITANCE=AR,AD,AD/AR;Gene_MIM_id=113705;Related_disorder_MIM_id=604370(SK)|614320(SK);Possible_Inheritance=AD;Haplo_Score=3;Triplo_Score=0	GT	0/1
```


## **Write configure file.**

Configure file should be wrote in json format. Please remember we have some reserved keywords. Just copy the demo.json in the example directory and edit your own configure file from 

```json
{
        "id":"configure ID and version",
        "author":"author of this configure file",
        "ref":"hg19",  // hg19 or hg38
        "hgvs":{
           "gene_data":"/opt/databases/refgene/hg19_refgene.tsv.gz",
           "refseq":"/opt/databases/refgene/refMrna.fa.gz",
        },
        "vcfs":[ 
          {
            "file":"path to clinvar.vcf.gz",
       	    "columns":"RS,CLNSIG",
          },
          {
     	    "file":"path to vcf database",
            "columns":"tags",
          },
        ],
        "beds":[
          {
            "file":"path to BED-like database",
            "columns":"tags",
          },          
        ],
}
```

## **Convert annotated vcf file to other formats.**

**vcf2tsv** is a part of bcfanno package, convert selected tags from VCF/BCF to tab-seperated file.  For the usage of vcf2tsv please refer to [vcf2tsv manual](https://github.com/shiquan/vcfanno/blob/master/documents/vcf2tsv_manual.md).

```
vcf2tsv -f BED,REF,ALT,GT,SAMPLE,Gene,HGVSnom,ExonIntron,VarType,HGMD_tag example/demo_anno.vcf

// results 
#CHROM	START	END	REF	ALT	GT	SAMPLE	Gene	HGVSnom	ExonIntron	VarType	HGMD_tag
17	41222825	41222826	A	C	0/1	demo	BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1	NM_007294.3:c.4987-3114A>C|NM_007297.3:c.4846-3114A>C|NM_007298.3:c.1675-3114A>C|NM_007299.3:c.1675-3114A>C|NM_007300.3:c.5050-3114A>C|NR_027676.1:n.5123-3114A>C	I15|I14|I14|I15|I16|I15intron|intron|intron|intron|intron|intron	.
17	41223241	41223242	G	C	0/1	demo	BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1	NM_007294.3:c.4689C>G(p.Tyr1563Stop/p.Y1563X)|NM_007297.3:c.4548C>G(p.Tyr1516Stop/p.Y1516X)|NM_007298.3:c.1377C>G(p.Tyr459Stop/p.Y459X)|NM_007299.3:c.1377C>G(p.Tyr459Stop/p.Y459X)|NM_007300.3:c.4752C>G(p.Tyr1584Stop/p.Y1584X)|NR_027676.1:n.4825C>G	E15/C14|E14/C12|E14/C14|E15/C14|E16/C15|E15/C15	nonsense|nonsense|nonsense|nonsense|nonsense|noncoding	DM
17	41234450	41234451	G	A	0/1	demo	BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1	NM_007294.3:c.4327C>T(p.Arg1443Stop/p.R1443X)|NM_007297.3:c.4186C>T(p.Arg1396Stop/p.R1396X)|NM_007298.3:c.1018C>T(p.Arg340Stop/p.R340X)|NM_007299.3:c.1018C>T(p.Arg340Stop/p.R340X)|NM_007300.3:c.4327C>T(p.Arg1443Stop/p.R1443X)|NR_027676.1:n.4463C>T	E12/C11|E11/C9|E11/C11|E12/C11|E12/C11|E12/C12	nonsense|nonsense|nonsense|nonsense|nonsense|noncoding	DM
17	41258325	41258326	A	G	0/1	demo	BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1	NM_007294.3:c.213-1353A>G|NM_007297.3:c.72-1353A>G|NM_007298.3:c.213-1353A>G|NM_007299.3:c.213-1353A>G|NM_007300.3:c.213-1353A>G|NR_027676.1:n.352-1353A>G	I4|I3|I3|I4|I4|I4	intron|intron|intron|intron|intron|intron	.
17	41258503	41258504	A	C	0/1	demo	BRCA1|BRCA1|BRCA1|BRCA1|BRCA1|BRCA1	NM_007294.3:c.181T>G(p.Cys61Gly/p.C61G)|NM_007297.3:c.40T>G(p.Cys14Gly/p.C14G)|NM_007298.3:c.181T>G(p.Cys61Gly/p.C61G)|NM_007299.3:c.181T>G(p.Cys61Gly/p.C61G)|NM_007300.3:c.181T>G(p.Cys61Gly/p.C61G)|NR_027676.1:n.342T>G	E4/C3|E3/C1|E3/C3|E4/C3|E4/C3|E4/C4	missense|missense|missense|missense|missense|noncoding	DM
```


## **Interpret the annotations.**

Please refer  to (ACMG guideline)[https://www.google.com.hk/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=0ahUKEwigz5jkzLHVAhUBG5QKHesKAhYQFggkMAA&url=https%3A%2F%2Fwww.acmg.net%2Fdocs%2Fstandards_guidelines_for_the_interpretation_of_sequence_variants.pdf&usg=AFQjCNF_jV6xApr4TCyQhs9p0Iki85oYoQ] to interpret genetic variants.



## **Bug report or suggestions**.


## **Reference**

