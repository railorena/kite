args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]

input <- read.csv(paste0(conditions), header = F)
#input <- read.csv("../../../ella-scratch/kite/input_training.csv", header = F)

colnames(input) <- c("sample", "condition")
input <- input[order(input$condition),]

train <- read.csv("output.tsv", sep = "\t", row.names = 1)
#train <- read.csv("../../../ella-scratch/kite/output.tsv", sep = "\t", row.names = 1)
train <- train[ , colSums(is.na(train))==0]

train <- as.data.frame(t(train))
train <- train[,order(colnames(train))]

train$sample <- rownames(train)
train$class <- NA

for (i in 1:nrow(train)){
  #print(i)
  for (j in 1:nrow(input)) {
    if(train[i,"sample"] == input[j,"sample"]){
      train[i, "class"] <- input[j,"condition"]
    }
  }
}

train <- train[complete.cases(train), ]
train <- train[ , -which(names(train) %in% c("sample"))]

cond <- unique(train$class)
cond <- cond[order(cond)]
train[which(train$class == cond[1]),] <- 0
train[which(train$class == cond[2]),] <- 1

train$class <- as.numeric(train$class)

write.csv(train, file = "train_data.csv")