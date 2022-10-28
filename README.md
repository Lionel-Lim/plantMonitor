# Plant Monitor

## 1\. Project Overview

The project is developing the plant monitor collecting and visualising the factors affecting growth.  I will monitor [Ficus microcarpa Moclame](https://www.rhs.org.uk/plants/164300/ficus-microcarpa-moclame-(pbr)/details) using various components, for example, [Adafruit](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/overview) [Feather Huzzah ESP8266](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/overview)(Control Board), [DHT22](https://learn.adafruit.com/dht/overview)(Temperature & Humidity Sensor), [a pair of nails](https://www.instructables.com/Moisture-Detection-With-Two-Nails/) and so on. The collected data from the plant and soil will be sent to CASA [MQTT](https://en.wikipedia.org/wiki/MQTT) server, stored in [InfluxDB](https://en.wikipedia.org/wiki/InfluxDB) and visualised on [Grafana](https://en.wikipedia.org/wiki/Grafana).

  

## 2\. Understanding How Plant Grow

To get energy for growth, Plants absorb water inside the soil. By doing so, plants can absorb water and nutrients inside the soil. Then, the absorbed water goes to the leaves, and the plants use it with carbon dioxide and light for _photosynthesis_ to create chemical energy, such as carbohydrates and sugars. ([Photosynthesis - Wikipedia](https://en.wikipedia.org/wiki/Photosynthesis))

To monitor the factors affecting plant growth, It is essential to understand the mechanism of plant growth. Plant growth is generally affected by four factors. ([Environmental factors affecting plant growth | OSU Extension Service (oregonstate.edu)](https://extension.oregonstate.edu/gardening/techniques/environmental-factors-affecting-plant-growth))

Table 1 shows the growth factors and sensors to monitor how it changes.

|     |     |     |
| --- | --- | --- |
| _**Factors**_ | _**Description**_ | _**Sensors**_ |
| Light | Quantity, Quality and Duration can affect plant growth. | - [Adafruit AS7341](https://www.adafruit.com/product/4698)(Not in the project) |
| Temperature | The level of temperature affects germination, flowering and Crop Quality. | - DHT22 |
| Water and Humidity | Water inside the soil is the important ingredient for photosynthesis, and the nutrients of the soil are dissolved in it. The humidity in the air controls the transpiration of the plant. | - A pair of nails(Water in the soil)<br>-  DHT22(Humidity) |
| Plant Nutrition | Six soil elements called _macronutrients_ are largely used by plants. They are nitrogen(N), potassium(P), phosphorus(K), magnesium(Mg), calcium(Ca), and sulfur(S). | - [Soil NPK Sensor](https://www.circuitschools.com/measure-soil-npk-values-using-soil-npk-sensor-with-arduino/)(Not in the project)<br>- [Soil PH Sensor](https://www.renkeer.com/product/soil-ph-sensor/) |

Table 1. Factors of Plant Growth

  

## 3\. Sensors in the project

Two sensors(DHT22 and a pair of nails) are used in this project. Some sensors in Table 1 are not used now but those will be equipped on the module in future.

1. DHT22

- The sensor collects temperature and humidity data every two seconds.

1. A pair of nails

- The sensor collects resistivity data of the soil. when one nail put 5V to soil, another nail detect it. If a moisture level of the soil is high, the electricity can easily flow between one nail to another and it shows high value.

  

## 0\. Reference

[Moisture Detection With Two Nails : 3 Steps - Instructables](https://www.instructables.com/Moisture-Detection-With-Two-Nails/)  

[Photosynthesis - Wikipedia](https://en.wikipedia.org/wiki/Photosynthesis)  

[Environmental factors affecting plant growth | OSU Extension Service (oregonstate.edu)](https://extension.oregonstate.edu/gardening/techniques/environmental-factors-affecting-plant-growth)  

[Soil resistivity - Wikipedia](https://en.wikipedia.org/wiki/Soil_resistivity#:~:text=Soil%20resistivity%20is%20a%20measure,current%20through%20the%20Earth's%20surface.)