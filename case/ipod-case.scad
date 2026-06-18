// iPod Player Case - OpenSCAD
// Designed for RPi Zero 2W + custom HAT + 2" IPS display
// Print in PLA or PETG, 0.2mm layer height

// Dimensions (mm)
case_w = 62;      // width
case_d = 110;     // depth/height (portrait orientation)
case_t = 18;      // total thickness
wall = 1.6;       // wall thickness
corner_r = 4;     // corner radius
fillet = 1;       // edge fillet

// Component positions (from bottom of case interior)
pcb_z = 2;        // PCB standoff height
pcb_w = 65;       // HAT PCB width (slight overhang ok)
pcb_d = 56.5;     // HAT PCB depth
pcb_thick = 1.6;

// Display cutout
disp_w = 28;      // ST7789 2" display active area
disp_h = 35;
disp_x = (case_w - disp_w) / 2;
disp_y = 8;       // from top edge

// Headphone jack hole
jack_dia = 6.5;
jack_y = case_d - 5;  // near bottom edge

// USB-C charging port
usbc_w = 9;
usbc_h = 3.5;

// Encoder hole
enc_dia = 7;
enc_x = case_w / 2;
enc_y = case_d - 35;

// Button holes
btn_dia = 4;
btn_spacing = 12;
btn_y = case_d - 20;

// Speaker grille
speaker_dia = 20;
speaker_y = case_d - 55;

$fn = 60;

module rounded_box(w, d, h, r) {
    hull() {
        for (x = [r, w-r])
            for (y = [r, d-r])
                translate([x, y, 0])
                    cylinder(r=r, h=h);
    }
}

module case_shell() {
    difference() {
        // Outer shell
        rounded_box(case_w, case_d, case_t, corner_r);

        // Hollow interior
        translate([wall, wall, wall])
            rounded_box(case_w - 2*wall, case_d - 2*wall, case_t - wall, corner_r - wall);
    }
}

module display_cutout() {
    translate([disp_x, disp_y, -0.1])
        cube([disp_w, disp_h, wall + 0.2]);
}

module headphone_cutout() {
    translate([case_w - 3, jack_y, case_t/2])
        rotate([0, 90, 0])
            cylinder(d=jack_dia, h=wall + 2);
}

module usbc_cutout() {
    translate([case_w - 3, case_d - 15, pcb_z + pcb_thick])
        rotate([0, 90, 0])
            hull() {
                translate([0, 0, 0]) cylinder(d=usbc_h, h=wall + 2);
                translate([0, usbc_w - usbc_h, 0]) cylinder(d=usbc_h, h=wall + 2);
            }
}

module encoder_hole() {
    translate([enc_x, enc_y, -0.1])
        cylinder(d=enc_dia, h=wall + 0.2);
}

module button_holes() {
    for (i = [-1, 0, 1]) {
        translate([case_w/2 + i * btn_spacing, btn_y, -0.1])
            cylinder(d=btn_dia, h=wall + 0.2);
    }
}

module speaker_grille() {
    translate([case_w/2, speaker_y, case_t - wall - 0.1]) {
        for (a = [0:30:330]) {
            for (r = [3, 6, 9]) {
                translate([r * cos(a), r * sin(a), 0])
                    cylinder(d=1.5, h=wall + 0.2);
            }
        }
        cylinder(d=1.5, h=wall + 0.2);
    }
}

module pcb_standoffs() {
    // RPi HAT mounting hole positions
    standoff_h = pcb_z;
    standoff_od = 5;
    standoff_id = 2.2;  // M2.5 screw

    positions = [
        [3.5, 3.5],
        [61.5, 3.5],
        [3.5, 52.5],
        [61.5, 52.5]
    ];

    // Offset to center PCB in case
    pcb_offset_x = (case_w - pcb_w) / 2;
    pcb_offset_y = (case_d - pcb_d) / 2 + 10;  // shifted toward bottom

    for (pos = positions) {
        translate([pos[0] + pcb_offset_x, pos[1] + pcb_offset_y, wall]) {
            difference() {
                cylinder(d=standoff_od, h=standoff_h);
                translate([0, 0, -0.1])
                    cylinder(d=standoff_id, h=standoff_h + 0.2);
            }
        }
    }
}

// === Front half (display side) ===
module front_case() {
    split_z = case_t / 2;

    difference() {
        intersection() {
            case_shell();
            translate([-1, -1, -1])
                cube([case_w + 2, case_d + 2, split_z + 1]);
        }

        display_cutout();
        encoder_hole();
        button_holes();
    }
}

// === Back half ===
module back_case() {
    split_z = case_t / 2;

    difference() {
        intersection() {
            case_shell();
            translate([-1, -1, split_z])
                cube([case_w + 2, case_d + 2, case_t - split_z + 1]);
        }

        headphone_cutout();
        usbc_cutout();
        speaker_grille();
    }

    pcb_standoffs();
}

// === Render ===
// Uncomment one at a time for STL export:

// Front half
color("white") front_case();

// Back half (offset for viewing)
translate([case_w + 10, 0, 0])
    color("silver") back_case();
