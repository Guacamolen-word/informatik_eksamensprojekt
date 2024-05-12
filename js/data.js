
function test() {

    let request = new XMLHttpRequest();
    // Options are either temperature or humidity
    var post_data = "filter=temperature";
    request.open('POST', "/data", true); 

    // Webserver only supports one format for POST
    request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

    // Handle request once recieved
    request.onload = function(event) {
        // Check for errors
        if(request.status == 200)  {
            document.getElementById("jsondump").innerHTML += request.response; 
        }else{
            console.log("ERROR: " + request.status);
        }
    }

    request.send(post_data);

} 

