# LightSwarm-DistributedSystems

<ul>
  <li>This project is a distributed system of devices which can communicate with each other by sending broadcast messages to exchange sensor readings. The device with the highest reading, will then forward the readings to the Raspberry Pi as the server</li>
  <li>Then I used Node-RED to connect Raspberry Pi to Google Firebase. The data are then sent from Raspberry Pi to Firebase, and saved on a database</li>
  <li>Once they are in the Firebase, we can use any software to access it anytime from anywhere for any purpose. We can also use Node-RED to send some live messages to your phone in real time</li>
  <li>Here, I am sending data from Firebase whenever the database is updated, to Slack installed on smart phone. The data is also extracted from the Firebase to our laptop for further analysis.</li>

</ul>

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

```
Give examples
```

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Authors

* **Neha Sadhvi** - *Initial work* - [neheasadhvi](https://github.com/nehasadhvi)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
