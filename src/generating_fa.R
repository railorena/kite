train <- read.csv("output.tsv", sep = "\t", row.names = 1)
seqs <- row.names(train)

library(seqinr)
seqnames <- paste0("seq_",seqs)
seqs <- as.list(seqs)
names(seqs) <- seqnames
write.fasta(sequences=seqs,names=names(seqs),file.out="kmers.fa")
