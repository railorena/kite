args <- commandArgs(trailingOnly = TRUE)
samples <- args[1]
#pathway <- args[2]

input <- read.csv(paste0(samples), header = F)
#input <- read.csv("../../../ella-scratch/kite/input_test.csv", header = F)

colnames(input) <- c("sample", "condition")
input <- input[order(input$condition),]

#reading the files and generating a matrix couting
for (i in 1:nrow(input)) {
  #print(i)
  if(i == 1){
    matrix <- read.csv(paste0(input[i,1],"_canon.tsv"), header = F, sep = " ")
    colnames(matrix)[1] <- "tag"
    matrix <- matrix[order(matrix$tag),]
    colnames(matrix)[i+1] <- input[i,1]
  }else{
    tmp <- read.csv(paste0(input[i,1],"_canon.tsv"), header = F, sep = " ")
    tmp <- tmp[order(tmp$V1),]
    matrix <- cbind(matrix, tmp$V2)
    colnames(matrix)[i+1] <- input[i,1]
  }
}

rownames(matrix) <- matrix$tag
matrix <- matrix[,-1]

# correcting the counting, putting zero to count less than 4
library(dplyr)
matrix <- matrix %>% mutate(across(where(is.numeric), function(x) ifelse(x < 4, 0, x)))

matrix <- as.data.frame(t(matrix))

matrix <- matrix[,order(colnames(matrix))]

rm(tmp, i)

########################################

matrix$sample <- rownames(matrix)
matrix$class <- NA

for (i in 1:nrow(matrix)){
  #print(i)
  for (j in 1:nrow(input)) {
    if(matrix[i,"sample"] == input[j,"sample"]){
      matrix[i, "class"] <- input[j,"condition"]
    }
  }
}


matrix <- matrix[complete.cases(matrix), ]
matrix <- matrix[ , -which(names(matrix) %in% c("sample"))]

cond <- unique(matrix$class)
cond <- cond[order(cond)]
matrix[which(matrix$class == cond[1]),] <- 0
matrix[which(matrix$class == cond[2]),] <- 1

matrix$class <- as.numeric(matrix$class)

write.csv(matrix, file = "table_test.csv")
