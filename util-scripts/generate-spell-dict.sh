file=".aspell.en.pws"
echo "personal_ws-1.1 en $(wc -l ${file}.txt | cut -d' ' -f1)" > "${file}"
cat "${file}.txt" >> "${file}"
