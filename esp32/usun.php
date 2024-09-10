<?php
require("connect.php");

if (isset($_GET["ID"]))
{
	$ID = $_GET["ID"];

	$sql = "DELETE FROM uzytkownicy WHERE ID = $ID";
	$result = $conn->query($sql);
	
	
}
header("location: lista_uzytkownikow.php");
exit;

?>