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

if ($count == 0) {
	
  $sql = "INSERT INTO uzytkownicy(ID, Imie, Nazwisko) VALUES ('$ID','$imie','$nazwisko')";
  $result_add = mysqli_query($conn, "INSERT INTO uzytkownicy(ID, Imie, Nazwisko) VALUES ('$ID','$imie','$nazwisko')");
  
  if ($result_add == TRUE)
  echo "Dodano użytkownika do bazy danych";

  else
	  echo "Wystąpił błąd w operacji dodawania użytkownika". $conn->error;
}
else 
echo "Użytkownik o takim identyfikatorze już istnieje w bazie danych. Proszę spróbować ponownie!";	

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
				<form>
						<button type="submit" formaction="index.html" class="index_powrot bg-primary
text-white"> <i class="bi bi-arrow-clockwise bg-primary
text-white"></i>
						Powrót do strony głównej
						</button>
						<button type="submit" formaction="dodaj.html" class="dodaj_kolejnego bg-success text-white">
						<i class="bi bi-plus-square bg-success text-white "></i> Dodaj kolejnego użytkownika
					</button>
				</form>

</body>

</html>