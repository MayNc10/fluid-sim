#include "collisionobject.h"
#include "point.h"
#include <optional>
#include <cmath>
#include <cstdio>
#include "constants.h"

std::optional<std::tuple<Vector2d, Point, float>> CollisionLine::has_collided(const Point initial_position, const Vector2d incoming) const {
    // This is essentially asking whether the point from the initial point crosses this line
    CollisionLine movement_line = CollisionLine(initial_position, initial_position + incoming);
    if (this->intersects(movement_line)) {
        // How would the incoming object bounce?
        // First, where does the object land on the plane?
        // Ripped from https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
        // t = (q - p) X s / (r X s)
        Vector2d v12 = this->p2 - this->p1; // vector from p1 to p2
        float t = Vector2d::cross(this->p1 - initial_position, v12) / Vector2d::cross(incoming, v12);
        // Use t to find the intersection point
        Point intersect = initial_position + incoming * t;
        // Find the normal vector in the direction opposing the object
        // Base normal is (-dy, dx)
        Vector2d normal = Vector2d(-v12.get_y(), v12.get_x());
        // Invert incoming so it points away from the plane
        Vector2d inv_incoming = incoming * -1;
        // Make sure the normal is in the right direction
        if (Vector2d::dot(normal, inv_incoming) <= 0) {
            normal = normal * -1;
        }
        // If the angle the plane makes with the normal isn't close to 90 (i.e. is 270), reverse the v12 vector
        float norm_plane_theta = std::acos(Vector2d::dot(normal, v12) / (normal.magnitude() * v12.magnitude())) * 180 /  PI;
        if (norm_plane_theta - 90 > 10) { // 10 is our epsilon value
            v12 *= -1;
        }

        float inv_incoming_plane_theta = std::acos(Vector2d::dot(inv_incoming, v12) / (inv_incoming.magnitude() * v12.magnitude())) * 180 /  PI;

        // calculate the angle of incidence
        float cos_theta = Vector2d::dot(normal, inv_incoming) / (normal.magnitude() * inv_incoming.magnitude());
        float theta = std::acos(cos_theta);
        float theta_prime;
        // If we're on the right of the normal, rotate by 2pi - 2theta, otherwise 2theta
        if (inv_incoming_plane_theta > 90)
               theta_prime = 2 * theta;
        else
            theta_prime = 2 * PI - 2 * theta;

        // We don't have matrices, which should be fine
        Vector2d rot_mat_top = Vector2d(std::cos(theta_prime), -std::sin(theta_prime));
        Vector2d rot_mat_bot = Vector2d(std::sin(theta_prime), std::cos(theta_prime));

        Vector2d exit = inv_incoming * DAMPING;//Vector2d(Vector2d::dot(rot_mat_top, inv_incoming), Vector2d::dot(rot_mat_bot, inv_incoming)) * DAMPING;
        return std::tuple<Vector2d, Point, float>{exit, intersect, t};
    } else {
        return std::nullopt;
    }
}

// https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool CollisionLine::intersects(CollisionLine other) const {
    // We actually don't care about the collinear case (I don't think)
    int o1 = orientation(this->p1, this->p2, other.p1);
    int o2 = orientation(this->p1, this->p2, other.p2);
    if (o1 == o2) { return false; }

    int o3 = orientation(other.p1, other.p2, this->p1);
    int o4 = orientation(other.p1, other.p2, this->p2);
    return o3 != o4;
}

void CollisionLine::render(SDL_Renderer * renderer) {
    SDL_RenderDrawLine(renderer, this->p1.x, this->p1.y, this->p2.x, this->p2.y);
}
