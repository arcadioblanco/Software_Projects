<?php
require("connect.php");

/*Dane wprowadzone z formularza*/
$imie = $_POST['imie'];
$nazwisko = $_POST['nazwisko'];
$ID = $_POST['ID'];
if (strlen($ID)==10)
{
/*Sprawdzenie ile razy podane ID wystąpiło w bazie danych - jeżeli już jest to wyrzuć błąd. Jeśli nie, to dodaj do bazy danych*/
$result = mysqli_query($conn, "SELECT COUNT(ID) AS 'count' from uzytkownicy WHERE ID='$ID'");
$row = mysqli_fetch_array($result);
$count = $row['count'];

if ($count == 0) {
	
  $sql = "INSERT INTO uzytkownicy(ID, Imie, Nazwisko) VALUES ('$ID','$imie','$nazwisko')";
  $result_add = mysqli_query($conn, $sql);
  
  if ($result_add == TRUE)
  echo "Dodano użytkownika do bazy danych";

  else
	  echo "Wystąpił błąd w operacji dodawania użytkownika". $conn->error;
}
else 
echo "Użytkownik o takim identyfikatorze już istnieje w bazie danych. Proszę spróbować ponownie!";	
}
else 
echo "Błędny format numeru ID. Proszę spróbować ponownie";