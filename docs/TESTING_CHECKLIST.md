# Manual Testing Checklist

Use fictional records only. Tick each item after testing the actual build.

- [ ] App launches and all five tabs appear.
- [ ] Register a General patient.
- [ ] Register an Emergency patient and confirm highlighting.
- [ ] Reject blank name/contact.
- [ ] Reject non-numeric age and age > 120.
- [ ] Edit a patient.
- [ ] Delete a patient and use Undo.
- [ ] Schedule an appointment for a valid patient.
- [ ] Reject appointment for a missing patient ID.
- [ ] Cancel an appointment.
- [ ] Mark an appointment completed.
- [ ] Add two patients to queue and confirm FIFO call order.
- [ ] Linear search finds a patient by ID.
- [ ] Binary search finds a patient by ID.
- [ ] Recursive search finds a patient by partial name.
- [ ] Selection sort works by name, ID and age.
- [ ] Insertion sort works by name, ID and age.
- [ ] Reports show accurate category and appointment counts.
- [ ] Save data, close app, reopen and load saved data.
- [ ] Ctrl+F, Ctrl+S and Ctrl+Q work.
- [ ] Wait 30 seconds and confirm dashboard refresh.
