int privileges;

if (invalid_login())
  if (allow_guests())
    privileges = GUEST;
else
  privileges = ADMINISTRATOR;