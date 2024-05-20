// <Tobias>
class dayE {
    constructor(e) {
        this.element = document.createElement("td")
        this.element.setAttribute("id",e)
        this.element.setAttribute("style", "border-radius: 6px; position: relative; border: 1px solid; padding: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none; cursor: pointer; text-align: center;")
        this.element.addEventListener("mouseover",this.addListener,true)
        this.element.addEventListener("mouseout",this.addListener2,true)
        //this.element.setAttribute("onmouseover","this.style.setProperty('background-color','aqua')")
        
        //this.element.setAttribute("onmouseout","this.style.setProperty('background-color','white')")
        //this.element.setAttribute("onclick",`console.log('hi')`)
    }

    get getColorBasedOfState() {
        const colorss = ['black','darkgrey']
        return colorss[this.state]
    }

    setDate(date,selectedMonth) {
        this.uhcl = Math.floor(date.valueOf() / 1000)
        //this.uhcl.setUTCSeconds(date)
        this.date = date.getDate()
        this.month = date.getMonth()
        this.element.innerHTML = this.date
        this.state = selectedMonth != this.month ? 1 : 0
        this.element.style.setProperty("color",this.getColorBasedOfState)
    }
    addListener() {
        this.style.setProperty('border-color','aqua')
    }
    addListener2() {
        this.style.setProperty('border-color',`${this.style.color}`)
    }
    get hoverColor() {
        return 'aqua'
    }
    get defaultColor() {
        return 'white'
    }
}

class calendar {
    constructor(parent) {
        this.selection1 = undefined
        this.selection2 = undefined
        this.tileSelection = false
        this.firstDay = new Date()



        const divv = document.createElement('div')
        divv.setAttribute("style","display: inline-block; text-align: center; border: 1px solid; padding: 5px;")
        const but1 = document.createElement('button')
        but1.setAttribute("style","float:right; position: relative;")
        but1.innerHTML = '→'
        but1.addEventListener("click",(event) => {this.newMonth(this.firstDay.getFullYear(),this.firstDay.getMonth(),1);})
        
        this.text = document.createElement('span')
        this.text.innerHTML = 'erhhmm'
        this.text.setAttribute('style','text-align: center; margin: auto; width: fit-content;')
        divv.appendChild(this.text)

        const but2 = document.createElement('button')
        but2.setAttribute("style","float:left; position: relative;")
        but2.innerHTML = '←'
        but2.addEventListener("click",(event) => {this.newMonth(this.firstDay.getFullYear(),this.firstDay.getMonth(),-1);})
        divv.appendChild(but1)
        divv.appendChild(but2)
        const eTable = document.createElement("table")
        eTable.setAttribute('style','padding: 5px')

        const days = ['Ma','Ti','On','To','Fr','Lø','Sø']
        for (let i = 0; i < 7; i++) {
            var dayy = document.createElement("th")
            dayy.innerHTML = days[i]
            eTable.appendChild(dayy)
        }
        this.array = []
        for (let i = 0; i < 6; i++) {
            const eRow = document.createElement("tr")
            this.array.push(new Array())
            for (let j = 0; j < 7; j++) {
                const eDat = new dayE(j + i * 7)
                this.array[i].push(eDat)
                eDat.element.addEventListener("mouseover",(event) => this.onHoverWhenSelected(parseInt(event.srcElement.id)))
                eRow.appendChild(eDat.element)
            }
            eTable.appendChild(eRow)
        }
        
        this.month = this.firstDay.getMonth()
        this.year = this.firstDay.getFullYear()
        this.firstDay.setDate(1)
        const day = this.firstDay.getDay()
        let iter2 = 0
        let iter = this.nextDate(this.firstDay, -(day - 1))
        //console.log(iter);
        while (iter2 < 42) {
            let erhm = this.array[Math.floor(iter2 / 7)][iter2 % 7]
            erhm.setDate(iter,this.firstDay.getMonth())
            //this.array[Math.floor(iter2 / 7)][iter2 % 7].element.setAttribute('onclick','onSelected(this)')

            erhm.element.addEventListener("click",(event) => {this.onSelected(this.array[Math.floor(event.srcElement.id / 7)][event.srcElement.id % 7])})

            iter = this.nextDate(iter, 1)
            iter2++
        }
        this.displayDate()
        // end
        divv.appendChild(eTable)
        parent.appendChild(divv)
    }

    get fromTo() {
        return this.array[Math.floor(this.selection1 / 7)][this.selection1 % 7].uhcl + ";" + this.array[Math.floor(this.selection2 / 7)][this.selection2 % 7].uhcl
    }

    nextDate(date, yes) {
        var newDay = new Date(date)
        newDay.setDate(newDay.getDate() + yes)
        return newDay
    }
    onSelected(key) {
        if (!this.tileSelection) {
            this.tileSelection = true
            this.clearSelection()
            key.element.style.setProperty("background-color","chartreuse")
            key.element.style.setProperty("border-color","blue")
            this.selection1 = parseInt(key.element.id)
            for (let i = 0; i < 6; i++) {   
                this.array[i].map((element) => {
                    element.element.removeEventListener("mouseover",element.addListener,true);
                    element.element.removeEventListener("mouseout",element.addListener2,true);
                    //element.element.addEventListener("mouseover",(event) => this.onHoverWhenSelected(parseInt(event.srcElement.id)))
                    //element.element.setAttribute("onmouseover","this.style.setProperty('background-color','aqua'); onHoverWhenSelected(this)")
                })
            }
        } else {
            this.tileSelection = false
            key.element.style.setProperty("border-color","blue")
            this.selection2 = parseInt(key.element.id)
            for (let i = 0; i < 6; i++) {   
                this.array[i].map((element) => {
                    element.element.addEventListener("mouseover",element.addListener,true);
                    element.element.addEventListener("mouseout",element.addListener2,true);
                    //element.element.setAttribute("onmouseover","this.style.setProperty('background-color','aqua'); onHoverWhenSelected(this)")
                })
            }
            console.log(this.fromTo);
        }
    }

    onHoverWhenSelected(td) {
        if (this.tileSelection) {
            if (td > this.selection1) {
                let iter = 0
                while (iter < 42) {
                    const erhm = this.array[Math.floor(iter / 7)][iter % 7]
                    if (iter >= this.selection1 && iter <= td) {
                        erhm.element.style.setProperty("background-color","chartreuse")
                    } else {
                        erhm.element.style.setProperty("background-color","white")
                    }
                    iter++
                }
            } else {
                let iter = 0
                while (iter < 42) {
                    const erhm = this.array[Math.floor(iter / 7)][iter % 7]
                    if (iter <= this.selection1 && iter >= td) {
                        erhm.element.style.setProperty("background-color","chartreuse")
                    } else {
                        erhm.element.style.setProperty("background-color","white")
                    }
                    iter++
                }
            }
        }
    }
    newMonth(year, month, wards) {
        let newYear = (month == 11 && wards > 0 || month == 0 && wards < 0) ? year + wards : year
        let newMonth = (year != newYear) ? ((year < newYear) ? 0 : 11) : month + wards
        let firstDay = new Date(newYear,newMonth,1)
        this.firstDay = firstDay
        //this.nextDate(new Date(),28)
        //firstDay.setDate(1)
        const day = firstDay.getDay()
        let iter2 = 0
        let iter = this.nextDate(firstDay, -((day == 0 ? 7 : day) - 1))
        while (iter2 < 42) {
            const erhm = this.array[Math.floor(iter2 / 7)][iter2 % 7]
            erhm.setDate(iter,firstDay.getMonth())
            erhm.element.style.setProperty('color',erhm.getColorBasedOfState)

            //this.array[Math.floor(iter2 / 7)][iter2 % 7].element.setAttribute('onclick','onSelected(this)')

            //this.array[Math.floor(iter2 / 7)][iter2 % 7].element.addEventListener("click",(event) => {this.onSelected(this.array[Math.floor(event.srcElement.id / 7)][event.srcElement.id % 7])})

            iter = this.nextDate(iter, 1)
            iter2++
        }
        this.displayDate()
    }
    displayDate() {
        const months = ['Jan','Feb','Mar','Apr','Maj','Jun','Jul','Aug','Sep','Okt','Nov','Dec']
        this.text.innerHTML = months[this.firstDay.getMonth()] + ' ' + this.firstDay.getFullYear()
    }
    clearSelection() {
        for (let i = 0; i < 6; i++) {
            this.array[i].map((element) => element.element.style.setProperty('background-color','white'))
        }
        this.selection1 = undefined
        this.selection2 = undefined
    }
}
