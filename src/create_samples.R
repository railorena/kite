args <- commandArgs(trailingOnly = TRUE)
conditions <- args[1]
pathway <- args[2]

set.seed(42)

input <- read.csv(conditions, header = TRUE)

# Standardize column names
colnames(input) <- c("samples_id", "condition")
input <- input[order(input$condition), ]

# Create the samples dataframe
samples <- data.frame(
  sample_str = paste0(
    input$samples_id, ": ",
    pathway, input$samples_id, "_1.fastq.gz; ",
    pathway, input$samples_id, "_2.fastq.gz!"
  )
)

# Write the samples file
write.table(samples, file = "samples", row.names = FALSE, col.names = FALSE, quote = FALSE)
