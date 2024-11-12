args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]
#pathway <- args[2]

input <- read.csv(paste0(conditions), header = F)
#input <- read.csv("../../../ella-scratch/kite/input_training.csv", header = F)

colnames(input) <- c("samples_id", "condition")
input <- input[order(input$condition),]

samples <- as.data.frame(NA)
for (i in 1:nrow(input)){
  samples[i,1] <- paste0(input[i,1],": training/",input[i,1],"_1.fastq.gz; ","training/",input[i,1],"_2.fastq.gz!")
}

write.table(samples, file = "samples", row.names = F, col.names = F, quote = F)