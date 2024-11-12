args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]
#pathway <- args[2]

input <- read.csv(paste0(conditions), header = F)
#input <- read.csv("../../../ella-scratch/kite/input_test.csv", header = F)

colnames(input) <- c("samples_id", "condition")
input <- input[order(input$condition),]


write.table(input$samples_id, file = "samples_cond", sep = ",", quote = F, row.names = F, col.names = F)
