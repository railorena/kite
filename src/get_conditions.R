args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]

set.seed(42)

input <- read.csv(conditions, header = TRUE)

# Standardize column names
colnames(input) <- c("samples_id", "condition")
input <- input[order(input$condition), ]


# Get unique conditions
cond <- unique(input$condition)

# Open file connection
con <- file("samples_cond", open = "w")

# Write each condition as a row
for (c in cond) {
  samples <- input$samples_id[input$condition == c]
  line <- paste(samples, collapse = ",")  # separated by comma
  writeLines(line, con)
}

close(con)
