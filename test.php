<?php
// Database credentials
$servername = "localhost";
$username = "root"; // Replace with your MySQL username
$password = ""; // Replace with your MySQL password
$database = "foodonate"; // Replace with your MySQL database name

// Create connection
$conn = new mysqli($servername, $username, $password, $database);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Get sensor data from the HTTP POST request sent by ESP32
$temperature = isset($_POST['temperature']) ? $_POST['temperature'] : null;
$moisture = isset($_POST['moisture']) ? $_POST['moisture'] : null;
$methane = isset($_POST['mq2']) ? $_POST['mq2'] : null;
$co2 = isset($_POST['mq135']) ? $_POST['mq135'] : null;

// Determine spoilage based on gas value threshold (e.g., 5000 for methane, 1000 for CO2)
$spoilt = ($methane >= 5000 || $co2 >= 1000) ? 1 : 0;  // 1 for spoiled, 0 for not spoiled

// Check if all data is provided
if ($temperature !== null && $moisture !== null && $methane !== null && $co2 !== null) {
    // Prepare and bind SQL statement
    $stmt = $conn->prepare("INSERT INTO sensor_data (temperature, moisture, methane, co2, spoilt) VALUES (?, ?, ?, ?, ?)");
    // Use 'd' for float (temperature) and 'i' for integers (moisture, methane, co2, spoilt)
    $stmt->bind_param("diiii", $temperature, $moisture, $methane, $co2, $spoilt);

    // Execute SQL statement
    if ($stmt->execute()) {
        echo "Data inserted successfully";
    } else {
        echo "Error inserting data: " . $stmt->error;
    }

    // Close connections
    $stmt->close();
} else {
    echo "Error: Sensor data not provided.";
}

$conn->close();
?>
