# setwd("/path/to/directory/")
setwd("~/Escritorio/")
data <- read.csv("drake2limpio2.csv")
data.mean <-
  aggregate(data[, 2:7], list(DateTime = data$DateTime), mean)
data.mean[, 2:4] <- round(data.mean[, 2:4], 2)
data.mean[, 5:6] <- round(data.mean[, 5:6], 0)
data.mean[, 7] <- round(data.mean[, 7], 2)
write.csv(data.mean, file = "drake2mean.csv", row.names = FALSE)

data.plots <- read.csv("drake2mean.csv")
data.plots$DateTime <-
  as.POSIXct(data.plots$DateTime, tz = "", "%Y-%m-%dT%H:%M:%S")

library(pastecs)
data.desc <- stat.desc(
  data.plots[, 2:7],
  basic = TRUE,
  desc = TRUE,
  norm = FALSE,
  p = 0.95
)
data.desc <- round(data.desc, 2)
print(data.desc)
write.csv(data.desc, file = "drake2desc.csv", row.names = TRUE)

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
      geom_point() +
      labs(title = plot.title, x = x.title, y = y.title) +
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
      )
    return(plot)
  }

tp <- PlotData(
  data.plots,
  data.plots$DateTime,
  data.plots$Temp,
  format(
    data.plots$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Temperature vs Date",
  "Date",
  "Temperature (°C)"
)
print(tp)

pp <- PlotData(
  data.plots,
  data.plots$DateTime,
  data.plots$Pressure,
  format(
    data.plots$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Atmospheric pressure vs Date",
  "Date",
  "Atmospheric pressure (hPa)"
)
print(pp)

hp <- PlotData(
  data.plots,
  data.plots$DateTime,
  data.plots$Humidity,
  format(
    data.plots$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Humidity vs Date",
  "Date",
  "Humidity (%)"
)
print(hp)

vlp <- PlotData(
  data.plots,
  data.plots$DateTime,
  data.plots$Visible,
  format(
    data.plots$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Visible light vs Date",
  "Date",
  "Visible light"
)
print(vlp)

irp <- PlotData(
  data.plots,
  data.plots$DateTime,
  data.plots$IR,
  format(
    data.plots$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "Infrared light vs Date",
  "Date",
  "Infrared light"
)
print(irp)

uvp <- PlotData(
  data.plots,
  data.plots$DateTime,
  data.plots$UV,
  format(
    data.plots$DateTime,
    format = "%d",
    tz = "",
    usetz = FALSE
  ),
  "UV index vs Date",
  "Date",
  "UV index"
)
print(uvp)
