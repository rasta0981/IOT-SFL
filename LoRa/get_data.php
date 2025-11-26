<?php
// Set the content type to JSON
header('Content-Type: application/json; charset=utf-8');
header("Access-Control-Allow-Origin: *"); // Allows the frontend HTML page to access this script

// Database credentials - Update these with your MySQL details
$servername = "localhost";
$username = "root"; // Default XAMPP username
$password = "****";     // Default XAMPP password (often blank)
$dbname = "sensors"; // The name of the database you created

// Initialize response array
$response = ["status" => "error", "message" => "An unknown error occurred", "data" => null];

// 1. Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// 2. Check connection
if ($conn->connect_error) {
    $response['message'] = "Connection failed: " . $conn->connect_error;
    echo json_encode($response);
    exit();
}

// 3. Define the SQL query
// This query selects the most recent single row from a 'readings' table.
// You must have a table named 'aht' with 'temperature' and 'humidity' and moisture columns.
$sql = "SELECT temperature, humidity, moisture FROM aht ORDER BY datetime DESC LIMIT 1";
$result = $conn->query($sql);

if ($result) {
    if ($result->num_rows > 0) {
        // Fetch the data
        $row = $result->fetch_assoc();
        
        $response['status'] = "success";
        $response['message'] = "Data retrieved successfully";
        $response['data'] = [
            "temperature" => (float)$row["temperature"], // Cast to float for JSON
            "humidity" => (float)$row["humidity"],
            "moisture" => (float)$row["moisture"],
            "timestamp" => $row["created_at"]
        ];
    } else {
        $response['message'] = "No records found in the database.";
    }
} else {
    $response['message'] = "Query failed: " . $conn->error;
}

// 4. Close connection
$conn->close();

// 5. Output the final JSON response
echo json_encode($response);
?>

