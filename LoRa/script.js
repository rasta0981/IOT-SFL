// Function to fetch data from the backend API
function fetchSensorData() {
    // *** CRITICAL: Update this URL to point to your actual backend script location. ***
    // If running on XAMPP, this might be: 
    // const apiUrl = 'http://localhost/aht/get_data.php'; 
       const apiUrl = 'get_data.php'; 

    const statusElement = document.getElementById('status');
    statusElement.textContent = 'Fetching data...';

    fetch(apiUrl)
        .then(response => {
            if (!response.ok) {
                // If the server responds with an error status (404, 500, etc.)
                throw new Error('Network response was not ok: ' + response.statusText);
            }
            return response.json(); // Parse the JSON data returned by the PHP script
        })
        .then(data => {
            // Check if the backend returned an expected structure and status
            if (data.status === 'success' && data.data) {
                // Update the HTML elements with the received data
                document.getElementById('temperatureValue').textContent = `${data.data.temperature}°F`;
                document.getElementById('humidityValue').textContent = `${data.data.humidity}%`;
                document.getElementById('moistureValue').textContent = `${data.data.moisture}%`;
                statusElement.textContent = 'Data updated successfully: ' + new Date().toLocaleTimeString();
            } else {
                throw new Error('Backend data format incorrect or status failed.');
            }
        })
        .catch(error => {
            // Handle any errors during the fetch operation (network down, PHP error, etc.)
            console.error('Error fetching sensor data:', error);
            statusElement.textContent = `Error connecting to backend: ${error.message}. Check your console and the API URL.`;
            document.getElementById('temperatureValue').textContent = '--';
            document.getElementById('humidityValue').textContent = '--';
            document.getElementById('moistureValue').textContent = '--';
        });
}

// Fetch data automatically when the page loads
document.addEventListener('DOMContentLoaded', (event) => {
    fetchSensorData();
});

