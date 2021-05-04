const char HTTP_WATER[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pl" style="height:100;">
    <head>
     <meta charset="UTF-8">
        <style>
            body { font-size: 1.2rem; line-height: 1.6; font-weight: 300;font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif; color: rgb(96, 60, 97); background-color: white;}
            .container {position: relative; margin: 0 auto;}
            .grid-container {position: relative;max-width: var(--grid-max-width);margin: 0 auto;padding: 10px;text-align: center;display: grid; grid-gap: 10px;gap: 10px;}
            .grid-container.quarters { grid-template-columns: repeat(4, 1fr); border-radius: 4px;}
            .grid-container.half { grid-template-columns: repeat(2, 1fr); border-radius: 4px;}
            section { border-radius: 8px; border: 2px solid rgb(22, 22, 22); margin-top: 5px; padding: 5px; max-width: 1024px; }
            .input {border-radius: 8px; border: 2px solid gray; width: 200px; height: 30px; font-size: 1.2rem; font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif; color: rgb(96, 60, 97); background-color: white;}
            .input_long {border-radius: 8px; border: 2px solid gray; width: 450px; height: 30px; font-size: 1.2rem; font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif; color: rgb(96, 60, 97); background-color: white;}
            .input_short {border-radius: 8px; border: 2px solid gray; width: 37px; height: 30px; font-size: 1.2rem; font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif; color: rgb(96, 60, 97); background-color: white;}
            .box {border-radius: 8px; width: 32px; height: 32px; border: 2px solid; color: rgb(96, 60, 97);}
            .submit_btn {border-radius: 8px; border: 2px solid gray; width: 980px; height: 30px; font-size: 1.2rem; font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif; color: white; background-color: rgb(96, 60, 97);}
        </style>
        <script type="text/javascript">

            window.onload = function (){
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    const response = JSON.parse(this.responseText);
                    for (const [key, value] of Object.entries(response)) {
                        let element = document.getElementById(key);
                        if (element){
                            if (element.type && element.type == "checkbox"){
                                element.checked = value;
                            } else {
                                element.value = value;
                            }
                        }
                    }                   
                }
                };
                xhttp.open("GET", "/json.htm", true);
                xhttp.send();
            }

            function refresh_data(){
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    const response = JSON.parse(this.responseText);
                    for (const [key, value] of Object.entries(response)) {
                        let element = document.getElementById(key);
                        if (element){
                            if (element.type && element.type == "checkbox"){
                            } else {
                                element.innerHTML = value;
                            }
                        }
                    }                   
                }
                };
                xhttp.open("GET", "/json.htm", true);
                xhttp.send();
            }
            refresh_data();
            setInterval(refresh_data, 3000);        
        </script>
    </head>
    <body>
    <center>
    <div class=\"container\">
        <header>
            <div>
                <img src="data:image/png;base64,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">
            </div>
        </header>
        <section>
            <div class="grid-container full">
                <table>
                    <tr>
                        <td width=70>
                            <a href="/">
                                <img src=" data:image/png;base64,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">
                            </a>
                        </td>
                        <td width=70>
                            <a href="/set_water">
                                <img src=" data:image/png;base64,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">
                            </a>
                        </td>
                        <td width=70>
                            <a href="/set_domo">
                                <img src=" data:image/png;base64,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">
                            </a>
                        </td>
                        <td width=70>
                            <a href="/set_telegram">
                                <img src=" data:image/png;base64,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">
                            </a>
                        </td>  
                        <td width=70>
                            <a href="/set_wifi">
                                <img src=" data:image/png;base64,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">
                            </a>
                        </td>                           
                        <td width=70>
                            <a href="/settings">
                                <img src=" data:image/png;base64,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">
                            </a>
                        </td>                                                                                                                        
                        <td>&nbsp;</td>
                        <td width=300><h2> <b id="gardner_name">wait...</b><b></b></h2></td
                    </tr>
                </table>
            </div>
        </section> 
        <section>
            <div class="grid-container full"><big><b>Ustawienia podlewania</b></big></div>
            <form action="/save_cfg" method="get">
            <div class="grid-container quarters">
                <div>
                    <label for="moisture_set">Nastawa Wilgotności (%)</label>
                    <input class="input" name="moisture_set" id="moisture_set" type="number" min="20" max="90" step="5" title="nastawa wilgotności w procentach - minimum 20%, maksimum 90%" placeholder="70">
                </div>
                <div>
                    <label for="tank_amount">Pojemność zbiornika (l)</label>
                    <input class="input" name="tank_amount" id="tank_amount" type="number" min="1" max="30" step="1" title="wielkość zbiornika na wodę w litrach" placeholder="10">
                </div>
                <div>
                    <label for="impulse_ml">Wielkość dawki (ml)</label>
                    <input class="input" name="impulse_ml" id="impulse_ml" type="number" min="100" max="500" step="50" title="ilość wody podawana w jednym cyklu pracy pompy" placeholder="250">
                </div>
                <div>
                    <label for="feed_interval">Interwał (min)</label>
                    <input class="input" name="feed_interval" id="feed_interval" type="number" min="5" max="30" title="interwał podlewania w minutach minimum 5, maksimum 30" placeholder="5">
                </div> 
            </div>
            <div>&nbsp;</div>
            <div>
                <input class="submit_btn" type="submit" value="zapisz" title="save">
            </div> 
            </form>   
            <div>
                &nbsp;
            </div>                     
        </section> 
        <section>
            <div class="grid-container full">
                <small><b>SQ9MDD @ 2021</b><br>
                    Edward Ogrodnik ESP8266 rozwiązanie dla roślin.
                    <br><b id="version">wait...</b>
                </small>
            </div>
        </section>  
    </div>
    </center>
    </body>
    </html>
)=====";