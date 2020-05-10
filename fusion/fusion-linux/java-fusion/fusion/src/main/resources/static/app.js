var stompClient = null;

function setConnected(connected) {
    $("#connect").prop("disabled", connected);
    $("#disconnect").prop("disabled", !connected);
    if (connected) {
        $("#conversation").show();
    }
    else {
        $("#conversation").hide();
    }
    $("#greetings").html("");
}

function connect() {

    var socket = new SockJS('/zhcx-export-websocket');
    stompClient = Stomp.over(socket);
    stompClient.connect({}, function (frame) {
        setConnected(true);
        console.log('Connected: ' + frame);
        stompClient.subscribe('/topic/zhcx', function (greeting) {
            console.log(greeting);
            showGreeting(JSON.parse(greeting.body).imageFusion);
        });
        // get session Id from url like "ws://localhost:8080/gs-guide-websocket/791/14pckehf/websocket"
        var sessionId = /\/([^\/]+)\/websocket/.exec(socket._transport.url)[1];
        console.log(sessionId);
        stompClient.subscribe('/user/' + sessionId + '/topic/zhcx', function (greeting) {
            console.log(greeting);
            showGreeting(JSON.parse(greeting.body).imageFusion);
        });
    });
}

function disconnect() {
    if (stompClient !== null) {
        stompClient.disconnect();
    }
    setConnected(false);
    console.log("Disconnected");
}

function sendName1() {
    stompClient.send("/zhcx/export", {}, JSON.stringify({'imageA': $("#name").val(), 'imageB': $("#name").val()}));
}

function sendName2() {
    stompClient.send("/zhcx/export", {}, JSON.stringify({'imageA': $("#name").val(), 'imageB': $("#name").val()}));
}

function showGreeting(message) {
    $("#greetings").append("<tr><td>" + message + "</td></tr>");
}

$(function () {
    $("form").on('submit', function (e) {
        e.preventDefault();
    });
    $("#connect").click(function () {
        connect();
    });
    $("#disconnect").click(function () {
        disconnect();
    });
    $("#send1").click(function () {
        sendName1();
    });
    $("#send2").click(function () {
        sendName2();
    });
});

