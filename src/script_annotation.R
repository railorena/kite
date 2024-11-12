library(Rsamtools)
library(jsonlite)
library(stringr)

bam <- scanBam("kmers_Aligned.out.bam")
sam <- read.csv("kmers_Aligned.out.sam", comment.char = "@", sep = "\t", header = F)

#getting the sequences with only one alignment
#NH:    Number of reported alignments that contain the query in the current record
#AS:	  Alignment score calculate by the aligner
bam <- bam[[1]]

bam_df <-  as.data.frame(bam$qname)
colnames(bam_df) <- "seqname"
bam_df$flag <- bam$flag
bam_df$chrm <- as.character(bam$rname)
bam_df$strand <- bam$strand
bam_df$start_kmer <- as.integer(bam$pos)
#bam_df$end_kmer <- as.integer(bam_df$start_kmer+30)
bam_df$qwidth <- bam$qwidth
bam_df$mapq <- bam$mapq
bam_df$cigar <- bam$cigar
bam_df$seq <- as.character(bam$seq)

colnames(sam)[1] <- "seqname"

sam <- sam[,c(1,12,13,14,15)]

seqs_selected <- merge(bam_df, sam, by="seqname")

rm(bam, sam, bam_df, pwd)

seqs_selected$original_seq <- str_remove_all(seqs_selected$seqname, "seq_")


seqs_selected <- seqs_selected[which(seqs_selected$seqname != "seq_class"),]


#100% match with only one alignment
annotated <- seqs_selected[which(seqs_selected$V12 == "NH:i:1" & seqs_selected$cigar == "31M"),]

#it is not 100%, but with only one alignment
# sequences_other <- seqs_selected[which(!seqs_selected$seqname %in% sequences_genes$seqname),]
# sequences_other <- sequences_other[which(sequences_other$V12 == "NH:i:1"),]
# rm(seqs_selected)
# 
# save(sequences_other, file = "table_genes/other_90p.RData")

unannotated <- seqs_selected[which(!seqs_selected$seqname %in% annotated$seqname),]
save(unannotated, file = "table_genes/unannotated_60p.RData")

unmapped <- unannotated[is.na(unannotated$cigar),]
save(unmapped, file = "table_genes/unmapped_60p.RData")
  
###### annotation
genes_table <- NA
n <- nrow(sequences_genes) #############################

for (i in 1:n) {
  print(i)
  gene_info <- NA
  exon_info <- NA
  region <- NA
  
  gene <- system(paste0("wget -q --header='Content-type:application/json' 'https://rest.ensembl.org/overlap/region/human/",
                        sequences_genes[i,"chrm"],":",sequences_genes[i,"start_kmer"],"-",sequences_genes[i,"end_kmer"],"?feature=gene;content-type=application/bed'  -O - "), intern = TRUE)
  exon <- system(paste0("wget -q --header='Content-type:application/json' 'https://rest.ensembl.org/overlap/region/human/",
                        sequences_genes[i,"chrm"],":",sequences_genes[i,"start_kmer"],"-",sequences_genes[i,"end_kmer"],"?feature=exon;content-type=application/bed'  -O - "), intern = TRUE)
  #Sys.sleep(1)
  gene_info <- tryCatch(fromJSON(gene), error = function(e) NULL)
  exon_info <- tryCatch(fromJSON(exon), error = function(e) NULL)
  
  #getting the kmers with gene name
  if(length(gene_info)>1)
    gene_info <- gene_info[complete.cases(gene_info), ]
  
  #checking if it is intron
  if(length(exon_info) == 0){
    region <- "intron"
  }else{
    #getting the same transcript
    if(nrow(exon_info)>1){
      tryCatch(
        if(list.any(exon_info$Parent == gsub("\\..*","",gene_info$canonical_transcript))){
          exon_info <- exon_info[which(exon_info$Parent == gsub("\\..*","",gene_info$canonical_transcript)),]
        }else{
          exon_info$rank <- "multiple"
        }
        , error = function(e) NULL)
    }
    tryCatch(
      if(list.any(exon_info$Parent == gsub("\\..*","",gene_info$canonical_transcript))){
        region <- exon_info[1,"rank"]
      }else{
        region <- exon_info[1,"rank"]
      }
      , error = function(e) NULL)
  }
  
  if(length(gene_info) == 15){
    gene_info$seq <- sequences_genes[i,"seq"]
    gene_info$original_seq <- sequences_genes[i,"original_seq"]
    gene_info$start_kmer <- sequences_genes[i,"start_kmer"]
    #gene_info$end_kmer <- sequences_genes[i,"end_kmer"]
    gene_info$chrm <- sequences_genes[i,"chrm"]
    gene_info$region <- region
    genes_table <- rbind(genes_table, gene_info)
  }
  
}
genes_table <- genes_table[-1,]

save(genes_table, file = "table_genes/genes_90p.RData")


#write.csv(sequences_other, file = "kmers_other_90p.csv", row.names = F)
