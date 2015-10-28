<?php
$servername = "localhost";
$username = "root";
$password = "root";
$dbname = "d20151024";
$tablename="children";

// Create connection
$conn = new mysqli($servername, $username, $password);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

// Create database
$sql = "CREATE DATABASE ".$dbname;
if ($conn->query($sql) === TRUE) {
    echo "Database created successfully";
} else {
    echo "Error creating database: " . $conn->error;
}

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

// sql to create table
$sql = "CREATE TABLE ".$tablename."(
id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
picture CHAR(30) ,
height INT,
color1 CHAR(30) ,
color2 CHAR(30) ,
time TIMESTAMP,
find BOOLEAN
)";

if ($conn->query($sql) === TRUE) {
    echo "Table MyGuests created successfully";
} else {
    echo "Error creating table: " . $conn->error;
}

$conn->close();
?>