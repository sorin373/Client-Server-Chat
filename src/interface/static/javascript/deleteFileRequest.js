document.addEventListener('DOMContentLoaded', () => {
    const table = document.querySelector('table');

    table.addEventListener('click', function (event) {
        const deleteButton = event.target.closest('.delete-btn');
        if (!deleteButton) return;

        event.preventDefault();
        deleteButton.disabled = true; // Disable the button during fetch

        const trElement = deleteButton.closest('tr');

        // Retrive the file ID from the left table column
        const fileId = trElement.querySelector('.left-column').textContent.trim();

        const requestBody = new URLSearchParams();
        requestBody.append('fileID', fileId);

        const headers = new Headers();
        headers.append('Content-Type', 'application/x-www-form-urlencoded');

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
            })
            .finally(() => {
                setTimeout(() => {
                    // Re-enable the button after a delay
                    deleteButton.disabled = false;
                }, 2000);
                
                // automatically refresh the page to see the changes
                setTimeout(function() {
                    location.reload();
                }, 2000);
            });
    });
});