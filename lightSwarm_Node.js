// Import Admin SDK
var firebase = require("firebase");

var config = {
  apiKey: "AIzaSyB6biGSaaHJt3_wlv52EtimUIf8ZVBLQC4",
  authDomain: "lightswarm-fc6b5.firebaseapp.com",
  databaseURL: "https://lightswarm-fc6b5.firebaseio.com",
  projectId: "lightswarm-fc6b5",
  storageBucket: "lightswarm-fc6b5.appspot.com",
  messagingSenderId: "355570751838"
};
firebase.initializeApp(config);

// Get a database reference to our posts
const db = firebase.database().ref();
const ref = db.child('RPi Readings');
const size = 10;
let arr = new Array(size).fill(0);
let i = total = avg = max = min = 0;

function calculateSliding(lightReading) {
  arr[i] = lightReading;
  avg = arr.reduce((acc, num) => num + acc, 0) / size;
  max = Math.max(...arr);
  min = Math.min(...arr);
  if(i++ == size-1) { i=0; }
  console.log(arr);
  console.log("Average is: ",avg);
  console.log("Maximum is: ",max);
  console.log("Minimum is: ",min);
}

// Attach an asynchronous callback to read the data at our posts reference
// Retrieve new posts as they are added to our database
ref.on("child_added", function(snapshot) {

  var newPost = snapshot.val();
  console.log("Master: " + newPost.masterIs);
  console.log("Readings: " + newPost.readingFromMaster);
  var data = parseInt(newPost.readingFromMaster);
  calculateSliding(data);
  console.log("******************");
});