args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]
#pathway <- args[2]

input <- read.csv(paste0(conditions), header = F)
#input <- read.csv("../../../ella-scratch/kite/input_training.csv", header = F)

colnames(input) <- c("samples_id", "condition")
input <- input[order(input$condition),]


cond <- unique(input$condition)
row1 <- data.frame(t(input[which(input$condition == cond[1]),1]))
row2 <- data.frame(t(input[which(input$condition == cond[2]),1]))

samples_cond <- dplyr::bind_rows(row1, row2)

write.table(samples_cond, file = "samples_cond", sep = ",", quote = F, row.names = F, col.names = F)
