<?php
require("connect.php");
if ($conn->connect_error) {
	die("Połączenie nieudane: " . $conn->connect_error);
}
$result = $conn->query("SELECT * from statusy");
?>


<!DOCTYPE HTML>

<html lang="pl">
<head>
<meta charset = "utf-8"/>
<link rel = "stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css"/>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">		<title>
Archiwum prób dostępu do pomieszczenia
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

<h1> Archiwum prób dostępu do pomieszczenia</h1>
<div class = "container my-5 bg-dark text-white">

<table class="table table-dark">
	<tr>
		<td> ID </td>
		<td> Imię</td>
		<td> Nazwisko</td>
		<td> Data </td>
		<td> Godzina </td>
		<td> Status próby </td>
	</tr>

	<?php
		while ($row = $result->fetch_assoc())
			{
				echo "
				<tr>
					<td> $row[ID]  </td>
					<td> $row[Imie] </td>
					<td> $row[Nazwisko]</td>
					<td> $row[Data]</td>
					<td> $row[Godzina]</td>
					<td> $row[Status]</td>
				</tr> 
				";	
			}
		
	?>	
</table>

</body>

</html>