
var ctx = document.getElementById('graph-container').getContext('2d');
var chart = new Chart(ctx, {
  type: 'line',

  data: {
    labels: [],
    datasets: [{
      label: 'Temperature (°C)',
      data: [],
      backgroundColor: 'transparent',
      borderColor: '#a3a3a3',
      borderWidth: 3,
      yAxisID: 'temperature',
      pointRadius: 0,
    }, {
      label: 'Humidity (%)',
      data: [],
      backgroundColor: 'transparent',
      borderColor: '#6893d9',
      borderWidth: 3,
      yAxisID: 'humidity',
      pointRadius: 0,
    }]
  },

  options: {
    scales: {
      xAxes: [{
        scaleLabel: {
          display: true,
          labelString: 'Timestamp',
        },
        min: '2023-01-09T13:20:00',
        max: '2023-01-09T13:50:00'
      }],
      yAxes: [{
        id: 'temperature',
        type: 'linear',
        position: 'left',
        ticks: {
          //min: 18,
          //max: 32,
          stepSize: 2,
          fontColor: '#a3a3a3',
        },
        
      }, {
        id: 'humidity',
        type: 'linear',
        position: 'left',
        ticks: {
          //min: 0,
          //max: 100,
          stepSize: 25,
          fontColor: '#6893d9',
        }
      }]
    },
    legend: {
      display: true,
      position: 'top',
    }
  }
});


function updateChartData() {
  $.ajax({
    url: 'data.php',
    success: function(json_data) {
      var data = JSON.parse(json_data);

      //console.log(json_data);
      //var data = [["timestamp","temperature"],["2023-01-08T19:45:38","22.8"],["2023-01-08T19:45:39","22.2"],["2023-01-08T19:45:59","22.8"],["2023-01-08T19:46:04","22.8"],["2023-01-08T19:46:05","22.8"],["2023-01-08T19:46:06","22.8"],["2023-01-08T19:46:14","22.9"]];
      // Update the chart data
     // Update the chart data
    chart.data.labels = data.map(function(d) { return d[0]; });
    chart.data.datasets[0].data = data.map(function(d) { return d[1]; });
    chart.data.datasets[1].data = data.map(function(d) { return d[2]; });
    chart.update();
    }
  });
}

updateChartData();
setInterval(updateChartData, 5000);