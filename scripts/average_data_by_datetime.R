input.file <- readline("Enter file path: ")

AverageDataByDateTime <- function(input.file) {
  # Averages data by date and time
  #
  # Args:
  #   input.file: file to read.
  #
  # Returns:
  #   Averaged data frame
  data.mean <-
    aggregate(input.data[, 2:7], list(DateTime = input.data$DateTime), mean)
  data.mean[, 2:4] <- round(data.mean[, 2:4], 2)
  data.mean[, 5:6] <- round(data.mean[, 5:6], 0)
  data.mean[, 7] <- round(data.mean[, 7], 2)
  write.csv(
    data.mean,
    file = paste(strsplit(
      input, split = ".", fixed = TRUE
    )[[1]][1],
    "mean.csv", sep = ""),
    row.names = FALSE
  )
  return(data.mean)
}

data.mean = AverageDataByDateTime(input.file)
