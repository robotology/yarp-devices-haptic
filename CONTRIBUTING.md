
#### How to add up a new physical haptic device driver

1. **Fork** this repository.
2. Add up the new haptic device within the **drivers directory** following the guidelines you can figure out from the existing drivers.
3. Add `add_subdirectory(new_driver)` cmake directive in the `drivers/CMakeLists.txt` file.
4. Carry out **extensive tests**.
5. Make the **pull request**.
