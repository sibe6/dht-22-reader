<?php
  $data = array();
  if (($handle = fopen("/var/temperature/data.csv", "r")) !== FALSE) {
    while (($row = fgetcsv($handle, 1000, ",")) !== FALSE) {
      $timestamp = $row[0];
      $temperature = $row[1];
      $data[] = array($timestamp, $temperature);
    }
    fclose($handle);
  }
  $last_data = array_pop($data);

  $json_lastdata = json_encode($last_data);
  echo $json_lastdata;
  exit;
?>