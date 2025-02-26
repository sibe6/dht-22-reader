<?php
  $data = array();
  if (($handle = fopen("/var/temperature/data.csv", "r")) !== FALSE) {
    while (($row = fgetcsv($handle, 1000, ",")) !== FALSE) {
      $timestamp = $row[0];
      $temperature = $row[1];
      $humidity = $row[2];
      $data[] = array($timestamp, $temperature, $humidity);
    }
    fclose($handle);
  }

  $json_data = json_encode($data);
  echo $json_data;
  exit;
?>