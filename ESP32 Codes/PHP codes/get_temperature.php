<?php

$location="localhost";
$user="root";
$pass="";
$db_name="esp32_thermometer";

// connect db
$conn = new mysqli($location, $user, $pass, $db_name);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
//echo "Connected successfully";

// $query1=" INSERT INTO count SET 
//           barcode='".$_GET['barcode']."',
//           count='".$_GET['count']."',
//           date='".date('Y-m-d H:i:s')."' ";  
// if(mysqli_query($db,$query1)){
//    echo "#yes";
// }
// else {
//    echo "#no";
// }

// $sql = "INSERT INTO readings SET
//         personid='".$_GET['inputName']."',
//         datetime='".date('Y-m-d H:i:s')."',
//         temperature='".$GET['send_temperature']."' ";

$sql = "INSERT INTO readings SET
        personid='".$_GET['inputName']."',
        temperature='".$_GET['send_temperature']."',
        datetime='".date('Y-m-d H:i:s')."' "; 

    
if ($conn->query($sql) === TRUE) {
    echo "New record created successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

$conn->close();
 
?>