var newJournal = null;
var configDialog = null;
var journals  = {};
var database = null;

function isObject(obj) {
  return obj === Object(obj);
}
function fail(reason) {
    $("#overlay_data").html(
        '<div class="centered"><img src="img/fail.png"><br>'+reason+'</div>');
    $("#overlay").show();
}

function doAddJournal(name, getimages) {
    if (journals[name] !== undefined)
        return;

    journals[name] = {

    };

}

function resizeDesk(n) {
    if (n < 100) n = 100;
    if (n > window.width-100) n = window.width-100;

    var l = n+"px";
    var r = (n+6)+"px";
    document.getElementById("left").style.width=l;
    document.getElementById("right").style.left=r;
    document.getElementById("deskhandle").style.left=l;
    window.localStorage.gui_split = n;
}
$(document).ready(function() {
    /***************************************************************/
    /* init splitter */
    var n = Number(window.localStorage.gui_split);
    if (isNaN(n)) n = 200;
    resizeDesk(n);
    $("#deskhandle").draggable({
        axis: "x",
        drag: function(event, ui) {
            resizeDesk(ui.position.left);
        },
        drop: function(event, ui) {
            resizeDesk(ui.position.left);
        }
    });
    /***************************************************************/
    /* init database */
    var dbreq = window.indexedDB.open("livejournal.offine");
    dbreq.onerror = function() {
        fail("Ошибка инициализации базы данных");
    }
    dbreq.onupgradeneeded = function(event) {
        var db = event.target.result;
        db.onerror = function(event) {
            fail("Ошибка инициализации базы данных");
        }
        var objectStore = db.createObjectStore("journals");
        var imageStore  = db.createObjectStore("images");
        var indexStore  = db.createObjectStore("index");


    }

    dbreq.onsuccess = function() {
        database = dbreq.result;
    }
    /***************************************************************/
    /* init journals list */
    try {
        if (window.localStorage.journals) {
            var j = Object(window.localStorage.journals);
            foreach(jj in j)
                addJournal(jj);
        }
    } catch(e) {
        //oops! journals are not initialized
    }
    /***************************************************************/
    /* init dialogs */
    newJournal = $("#add_journal").dialog({
        autoOpen: false,
        title: "Добавить журнал",
        minWidth: 400,
        open: function(event, ui) {
            $("#warning1").hide();
            var text = document.getElementById("journal_name");
            text.value = "";
            var saveimg1 = document.getElementById("saveimg1");
            var saveimg2 = document.getElementById("saveimg2");
            saveimg1.checked = saveimg2.checked;
        },
        buttons: [{
            text: "OK",
            click: function() {
                var text = document.getElementById("journal_name");
                if (journals[name] !== undefined) {
                    $("#warning1").html("Журнал уже добавлен").show();
                    return;
                }
                newJournal.dialog("close");
                doAddJournal(text.value,
                             document.getElementById("saveimg1").checked);
            }}]
    });
    configDialog = $("#conf_dialog").dialog({
        autoOpen: false,
        title: "Настройки",
        minWidth: 400,
        open: function(event, ui) {
            var text = document.getElementById("journal_name");
            text.value = "";
            var saveimg2 = document.getElementById("saveimg2");
            saveimg2.checked = Boolean(window.localStorage.saveimg);
        },
        buttons: [{
            text: "OK",
            click: function() {
                configDialog.dialog( "close" );
                var saveimg2 = document.getElementById("saveimg2");
                window.localStorage.saveimg = saveimg2.checked;
            }}]
    });

    $("#addj").click(function(){
        configDialog.dialog("close");
        newJournal.dialog("open");
    });
    $("#conf").click(function(){
        newJournal.dialog("close");
        configDialog.dialog("open");
    });
    $("#sync").click(function(){
        newJournal.dialog("close");
        configDialog.dialog("close");
    });
    $("#overlay").hide();
});

