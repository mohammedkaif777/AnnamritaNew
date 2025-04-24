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

// Query to fetch latest sensor data including id
$sql = "SELECT id, temperature, moisture, methane, co2, timestamp FROM sensor_data ORDER BY id DESC LIMIT 1";  
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    
    // Rename 'id' to 'serial' for JSON response
    $row['serial'] = $row['id'];
    unset($row['id']); // Remove 'id' from the array

    // Return data as JSON response
    echo json_encode($row);
} else {
    // Return empty data as JSON response
    echo json_encode(array());
}

$conn->close();
?>
