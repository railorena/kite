args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]

set.seed(42)

input <- read.csv(conditions, header = TRUE)

# Standardize column names
colnames(input) <- c("samples_id", "condition")
input <- input[order(input$condition), ]

train <- read.csv("output/matrix_kf.tsv", sep = "\t", row.names = 1)

train <- as.data.frame(t(train))
train <- train[,order(colnames(train))]

train$sample <- rownames(train)
train$class <- NA

for (i in 1:nrow(train)){
  for (j in 1:nrow(input)) {
    if(train[i,"sample"] == input[j,"samples_id"]){
      train[i, "class"] <- input[j,"condition"]
    }
  }
}

train <- train[complete.cases(train), ]
train <- train[ , -which(names(train) %in% c("sample"))]

cond <- unique(train$class)
cond <- cond[order(cond)]
train[which(train$class == cond[1]), "class"] <- 0
train[which(train$class == cond[2]), "class"] <- 1

train$class <- as.numeric(train$class)

write.csv(train, file = "output/train_data.csv")