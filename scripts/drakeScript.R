# setwd("/path/to/directory/")
setwd("~/tcu/")
input.file <- "drake2.csv"

data <- read.csv(input.file)
data$DateTime <-
  as.POSIXct(data$DateTime, tz = "", "%Y-%m-%dT%H:%M:%S")

library(pastecs)
data.desc <- stat.desc(
  data[, 2:7],
  basic = TRUE,
  desc = TRUE,
  norm = FALSE,
  p = 0.95
)
data.desc <- round(data.desc, 2)
write.csv(data.desc,
          file = paste(strsplit(
            input, split = ".", fixed = TRUE
          )[[1]][1], "desc.csv", sep = ""),
          row.names = TRUE)

library(ggplot2)
library(viridis)

PlotData <-
  function(df,
           x.data,
           y.data,
           plot.color,
           plot.title,
           x.title,
           y.title) {
    # Plots data against datetime
    #
    # Args:
    #   df: data frame.
    #   x.data: data for x axis.
    #   y.data: data for y axis.
    #   plot.color: color for plot.
    #   plot.title: title for plot.
    #   x.title: title for x axis.
    #   y.title: title for y axis.
    #
    # Returns:
    #   The plot for the specified data.
    plot <-
      ggplot(df,
             aes(x = x.data,
                 y = y.data,
                 color = plot.color)) +
      scale_color_viridis(discrete = TRUE,
                          direction = -1,
                          name = "Day") +
      theme(
        plot.title = element_text(size = 40),
        axis.text.x = element_text(size = 25),
        axis.text.y = element_text(size = 25),
        axis.title.x = element_text(size = 35),
        axis.title.y = element_text(size = 35),
        legend.title = element_text(size = 35),
        legend.text = element_text(size = 25),
        panel.background = element_rect(fill = "grey95"),
        panel.grid.major = element_line(colour = "white", size = 2),
        panel.grid.minor = element_line(colour = "white", size = 1)
      ) +
      geom_point() +
      labs(title = plot.title, x = x.title, y = y.title)
    return(plot)
  }

temp.plot <- PlotData(
  data,
  data$DateTime,
  data$Temp,
  format(
    data$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Temperature vs Date",
  "Date",
  "Temperature (°C)"
)
print(temp.plot)

press.plot <- PlotData(
  data,
  data$DateTime,
  data$Pressure,
  format(
    data$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Atmospheric pressure vs Date",
  "Date",
  "Atmospheric pressure (hPa)"
)
print(press.plot)

hum.plot <- PlotData(
  data,
  data$DateTime,
  data$Humidity,
  format(
    data$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Humidity vs Date",
  "Date",
  "Humidity (%)"
)
print(hum.plot)

visible.plot <- PlotData(
  data,
  data$DateTime,
  data$Visible,
  format(
    data$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Visible light vs Date",
  "Date",
  "Visible light"
)
print(visible.plot)

ir.plot <- PlotData(
  data,
  data$DateTime,
  data$IR,
  format(
    data$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Infrared light vs Date",
  "Date",
  "Infrared light"
)
print(ir.plot)

uv.plot <- PlotData(
  data,
  data$DateTime,
  data$UV,
  format(
    data$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "UV index vs Date",
  "Date",
  "UV index"
)
print(uv.plot)

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

AverageDataByDateTime("drake2.csv")
