document.addEventListener('DOMContentLoaded', () => {
    function search() {
        let input = document.getElementById("inputSearch");
        let filter = input.value.toUpperCase();
        let table = document.getElementById("tableID");
        let tr = table.getElementsByTagName("tr");

        for (let i = 0; i < tr.length; i++) {
            let td = tr[i].getElementsByTagName("td")[1];

            if (td) {
                let txtValue = td.textContent || td.innerText;

                if (txtValue.toUpperCase().indexOf(filter) > -1)
                    tr[i].style.display = "";
                else
                    tr[i].style.display = "none";
            }
        }
    }
    
    search();
    document.getElementById("inputSearch").addEventListener("keyup", search);
});
