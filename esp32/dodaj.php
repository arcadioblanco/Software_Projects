<?php
require("connect.php");

/*Dane wprowadzone z formularza w pliku dodaj.html*/
$imie = $_POST['imie'];
$nazwisko = $_POST['nazwisko'];
$ID = $_POST['ID'];

/*Sprawdzenie ile razy podane ID wystąpiło w bazie danych - jeżeli już jest to wyrzuć błąd. Jeśli nie, to dodaj do bazy danych*/
$result = mysqli_query($conn, "SELECT COUNT(ID) AS 'count' from uzytkownicy WHERE ID='$ID'");
$row = mysqli_fetch_array($result);
$count = $row['count'];

if ($count != 0)
echo "Użytkownik o takim numerze ID już istnieje w bazie danych. Proszę spróbować ponownie!";

else if (strlen($ID) != 10)
echo "Niepoprawny format numeru ID użytkownika. Proszę spróbować ponownie!";

else {
	
  $result_add = mysqli_query($conn, "INSERT INTO uzytkownicy(ID, Imie, Nazwisko) VALUES ('$ID','$imie','$nazwisko')");
  
  if ($result_add == TRUE)
  {
  header("location: index.html");
  exit;
  }
  else
	  echo "Wystąpił błąd w operacji dodawania użytkownika". $conn->error;
}
	
?>
<!DOCTYPE HTML>

<html lang="pl">
<head>
<meta charset = "utf-8"/>
<link rel = "stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css"/>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">		
		<title>
		Status dodania użytkownika
		</title>
		<style>
				body{
				background-color: black;
			}
			.index_powrot{
				margin-left:150px;
				margin-right:150px;
				margin-top:200px;
				height: 200px;
				width: 300px;
			}
			.dodaj_kolejnego{
				height: 200px;
				width: 300px;
			}
			
		</style> 
</head>

<body>

</body>

</html>