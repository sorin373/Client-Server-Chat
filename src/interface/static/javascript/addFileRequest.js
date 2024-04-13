document.addEventListener("DOMContentLoaded", () => {
    var isDownloadComplete = false;

    document.getElementById('addFileForm').addEventListener('submit', function (event) {
        event.preventDefault();

        var formData = new FormData(document.getElementById('addFileForm'));

        var xhr = new XMLHttpRequest();
        xhr.open('POST', '/addFile', true);

        xhr.upload.onprogress = function(event) 
        {
            if (event.lengthComputable) 
            {
                var percentComplete = (event.loaded / event.total) * 100;
                document.getElementById('uploadProgress').value = percentComplete;

                if (percentComplete === 100) 
                {
                    alert('File upload completed!');
                }
                else
                {
                    window.onbeforeunload = function() {
                        return "";
                    }
                }
            }
        };

        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) 
                if (xhr.status === 200) isDownloadComplete = true; 
                else console.error('File upload failed:', xhr.statusText);
        };

        xhr.send(formData);
    });
});