using AutoMapper;
using AutoMapper.QueryableExtensions;
using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web.Http;
using System.Web.Http.Description;
using Trivio.Models;
using Trivio.Models.DTOs;

namespace Trivio.Controllers
{
	[RoutePrefix("User")]
    public class UserController : ApiController
    {
        private TrivioContext db = new TrivioContext();

		//POST: User/Register
		[HttpPost]
		[Route("Register", Name = "RegisterUser")]
		[ResponseType(typeof(UserPublicDTO))]
		public async Task<IHttpActionResult> RegisterUser(UserRegistrationDTO newUser)
		{
			if (!ModelState.IsValid)
			{
				return BadRequest(ModelState);
			}

			//Add user to Db.
			var user = Mapper.Map<User>(newUser);
			try 
			{
				db.Users.Add(user);
				await db.SaveChangesAsync();
			}
			catch {
				return BadRequest(String.Format("Account with email {0} already exists",newUser.Email));
			}

			//Check Header for path to new resource
			return CreatedAtRoute(
				"RegisterUser", 
				new { id = user.Id},
				Mapper.Map<UserPublicDTO>(user)
			);
		}

		//POST: User/Login
		[HttpPost]
		[Route("Login",Name ="LoginUser")]
		[ResponseType(typeof(UserPublicDTO))]
		public async Task<IHttpActionResult> LoginUser(UserLoginDTO user)
		{
			User existingUser = await db.Users.FirstOrDefaultAsync(x=>(x.Email==user.Email && x.Password==user.Password));
			if (existingUser == null)
			{
				return NotFound();
			}

			return Ok(Mapper.Map<UserPublicDTO>(existingUser));
		}

		//GET: User/History/{UserId}
		[HttpGet]
		[Route("History/{id}",Name ="UserTriviaHistory")]
		public async Task<IHttpActionResult> GetUserTriviaHistory(int id)
		{
			//Check if User exists
			User user = await db.Users.FindAsync(id);
			if (user == null)
			{
				return BadRequest("Invalid UserId");
			}

			
			var userTriviaHistory = db.UserTriviaHistories
									.Include(b => b.Trivia)
									.Where(x => x.UserId == id)
									.Select(m=>new
										{
											Trivia = m.Trivia,
											UserAction=m.UserAction
										}
									).ToList();
			
			return Ok(new { 
				TriviaHistory = userTriviaHistory
			});
		}



		//----------------------------------------------------------------------------------

		// GET: Users
		public IQueryable<UserPublicDTO> GetUsers()
        {
            return db.Users.ProjectTo<UserPublicDTO>();
        }

        // GET: Users/5
        [ResponseType(typeof(User))]
        public async Task<IHttpActionResult> GetUser(int id)
        {
            User user = await db.Users.FindAsync(id);
            if (user == null)
            {
                return NotFound();
            }

            return Ok(Mapper.Map<UserPublicDTO>(user));
        }

        // PUT: User/5
        [ResponseType(typeof(void))]
        public async Task<IHttpActionResult> PutUser(int id, User user)
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            if (id != user.Id)
            {
                return BadRequest();
            }

            db.Entry(user).State = EntityState.Modified;

            try
            {
                await db.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!UserExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return StatusCode(HttpStatusCode.NoContent);
        }

        // DELETE: User/5
        [ResponseType(typeof(User))]
        public async Task<IHttpActionResult> DeleteUser(int id)
        {
            User user = await db.Users.FindAsync(id);
            if (user == null)
            {
                return NotFound();
            }

            db.Users.Remove(user);
            await db.SaveChangesAsync();

            return Ok(user);
        }
		
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }

        private bool UserExists(int id)
        {
            return db.Users.Count(e => e.Id == id) > 0;
        }
    }
}