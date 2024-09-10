<?php
require("connect.php");


if (!$conn) { 
	die("Connection failed: " . mysqli_connect_error()); 
} 


if(isset($_POST["wyslij_ID"])) {

	$ID = $_POST["wyslij_ID"];
    date_default_timezone_set("Europe/Warsaw"); 
    $time = date("H:i:s");
    $date = date("Y-m-d");

   $result = mysqli_query($conn, "SELECT COUNT(ID) AS 'count' from uzytkownicy WHERE ID='$ID'");
   $row = mysqli_fetch_array($result);
   $count = $row['count'];

    if ($count >0)
    {
        echo "OK";
        $status = "Przyjęto";
        $sql= "insert into statusy (ID, Imie, Nazwisko) select ID, Imie, Nazwisko from uzytkownicy where ID ='$ID'";
        mysqli_query($conn, $sql);
    }

    else
    {
        echo "NOK";
        $status = "Odrzucono";
        $sql= "insert into statusy (ID, Imie, Nazwisko) values ('$ID', '-','-')";
        mysqli_query($conn, $sql);
    }
   
    $sql2= "update statusy set Data = '$date', Godzina = '$time',Status='$status'  where ID='$ID'";
    mysqli_query($conn, $sql2);


}

