document.addEventListener("DOMContentLoaded", () => {
    document.getElementById('addFileForm').addEventListener('submit', function (event) {
        event.preventDefault();

        fetch('/addFile', {
            method: 'POST',
            body: new FormData(document.getElementById('addFileForm'))
        })
            .then(response => {
                if (response.ok) {
                    console.log('Form submitted successfully!');
                }
            })
            .catch(error => {
                console.error('Form submission failed:', error);
            });

        location.reload();

        alert('File added successfully!\nRefresh the page in order to see the changes.');
    });
});