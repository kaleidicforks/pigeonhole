require "editheader";
require "encoded-character";

# Ok
addheader "X-field" "Frop";

# Ok
addheader "X-field" "Frop
Frml";

# Invalid 'BELL'; but not an error
addheader "X-field" "Yeah${hex:07}!";

# Invalid 'NUL'
addheader "X-field" "Woah${hex:00}!";
