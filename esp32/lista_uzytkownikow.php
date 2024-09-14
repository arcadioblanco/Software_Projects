<?php
require("connect.php");
if ($conn->connect_error) {
	die("Połączenie nieudane: " . $conn->connect_error);
}
$result = $conn->query("SELECT * from uzytkownicy");
?>


<!DOCTYPE HTML>

<html lang="pl">
<head>
<meta charset = "utf-8"/>
<link rel = "stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css"/>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">		
<title>
			Lista użytkowników znajdujących się w bazie danych 
</title>

		 <style>
			body{
				background-color:black ;
			}
			h1{
				font-size: 20px;
				text-align: center;
				margin-bottom: 50px;
				font-family:Arial, Helvetica, sans-serif;
				color: white;
			}
			.usun{
				width:100px;
			}
		</style>
</head>

<body>

<h1> Lista użytkowników</h1>
<div class = "container my-5 bg-dark text-white">

<table class="table table-dark">
	<tr>
		<td> ID </td>
		<td> Imię</td>
		<td> Nazwisko</td>
		<td> Czy usunąć? </td>
	</tr>

	<?php
		while ($row = $result->fetch_assoc())
			{
				echo "
				<tr>
					<td> $row[ID]  </td>
					<td> $row[Imie] </td>
					<td> $row[Nazwisko]</td>
					<td> <a class='btn btn-danger btn-sm usun' href='usun.php?ID=$row[ID]'>Usuń</a> </td>
				</tr> 
				";	
			}
		
	?>	
</table>

</body>

</html>