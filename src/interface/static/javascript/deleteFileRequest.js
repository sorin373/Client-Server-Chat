document.addEventListener("DOMContentLoaded", () => {
    document.querySelector('table').addEventListener('click', function(event) {
        if (event.target.classList.contains('delete-btn')) {
            event.preventDefault();

            const trElement = event.target.closest('tr');
            const fileId = trElement.querySelector('.left-column').textContent.trim();

            const requestBody = new URLSearchParams();
            requestBody.append('fileID', fileId);

            const headers = new Headers();

            headers.append('Connection', 'closed');
            headers.append('Content-Type', 'application/x-www-form-urlencoded');
            headers.append('User-Agent', 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36');
            headers.append('Accept-Encoding', 'gzip, deflate');
            headers.append('Accept-Language', 'en-US,en;q=0.9,ro;q=0.8');

            fetch('/delete_file', {
                method: 'POST',
                body: requestBody,
                headers: headers
            })
            .then(response => {
                if (response.ok) {
                    console.log('File deletion request sent!');
                    alert('File deleted successfully!');
                } else {
                    console.error('File deletion request failed.');
                }
            })
            .catch(error => {
                console.error('Error:', error);
            });

            setTimeout(function() {
                location.reload();
            }, 2000);
        }
    });
});